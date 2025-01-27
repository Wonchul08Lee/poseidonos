/*-
 *   BSD LICENSE
 *
 *   Copyright (c) Intel Corporation.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "src/io/general_io/affinity_manager.h"
#include "src/scheduler/scheduler_api.h"

extern "C"
{

#include "spdk/stdinc.h"

#include "spdk/bdev.h"
#include "spdk/copy_engine.h"
#include "spdk/conf.h"
#include "spdk/env.h"
#include "spdk/thread.h"
#include "spdk/log.h"
#include "spdk/string.h"
#include "spdk/queue.h"
#include "spdk/util.h"

#include "spdk_internal/thread.h"

#define typeof __typeof__    
#include "config-host.h"    
#include "fio.h"
#include "optgroup.h"

#include "spdk/ibof.h"
#include "spdk/ibof_volume.h"
#include "spdk/event.h"

static struct fio_option options[4];

struct spdk_fio_options {
	void *pad;
	char *conf;
	unsigned mem_mb;
	bool mem_single_seg;
};

struct spdk_fio_request {
	struct io_u		*io;
	struct thread_data	*td;
    spdk_bdev_io		*bdev_io;
};

struct spdk_fio_target {
	struct spdk_bdev	*bdev;
	struct spdk_bdev_desc	*desc;
	struct spdk_io_channel	*ch;

	TAILQ_ENTRY(spdk_fio_target) link;
};

struct spdk_fio_thread {
	struct thread_data		*td; /* fio thread context */
	struct spdk_thread		*thread; /* spdk thread context */

	TAILQ_HEAD(, spdk_fio_target)	targets;
	bool				failed; /* true if the thread failed to initialize */

	struct io_u		**iocq;		/* io completion queue */
	unsigned int		iocq_count;	/* number of iocq entries filled by last getevents */
	unsigned int		iocq_size;	/* number of iocq entries allocated */
	unsigned int		io_reactor;   /* reactor dedicated for this fio thread */
};

static bool g_spdk_env_initialized = false;
static pthread_mutex_t g_reactor_mtx= PTHREAD_MUTEX_INITIALIZER;
static unsigned int g_last_assigned_reactor = UINT32_MAX;

static char *volume_name;
static char *setup_path;

static int spdk_fio_init(struct thread_data *td);
static void spdk_fio_cleanup(struct thread_data *td);
static size_t spdk_fio_poll_thread(struct spdk_fio_thread *fio_thread);

/* defined in main.cpp. */
int intialize_fio_plugin_bdev(); 


/* Default polling timeout (ns) */
#define SPDK_FIO_POLLING_TIMEOUT 1000000000ULL

uint32_t
get_io_reactor()
{
	pthread_mutex_lock(&g_reactor_mtx);

	if(g_last_assigned_reactor == UINT32_MAX) //not initialized?
	{
		g_last_assigned_reactor = spdk_env_get_first_core();
	}

    else
    {        
    	g_last_assigned_reactor = spdk_env_get_next_core(g_last_assigned_reactor);

	    if(g_last_assigned_reactor == UINT32_MAX)
    	{
	    	g_last_assigned_reactor = spdk_env_get_first_core();
    	}
    }        

	pthread_mutex_unlock(&g_reactor_mtx);
	return g_last_assigned_reactor;
}	

static int
spdk_fio_init_thread(struct thread_data *td)
{
	struct spdk_fio_thread *fio_thread;

	fio_thread = (struct spdk_fio_thread *)calloc(1, sizeof(*fio_thread));
	if (!fio_thread) {
		SPDK_ERRLOG("failed to allocate thread local context\n");
		return -1;
	}

	fio_thread->td = td;
	td->io_ops_data = fio_thread;

	fio_thread->thread = spdk_thread_create_without_registered_fn("fio_thread");

	if (!fio_thread->thread) {
		free(fio_thread);
		SPDK_ERRLOG("failed to allocate thread\n");
		return -1;
	}
	spdk_set_thread(fio_thread->thread);

	fio_thread->iocq_size = td->o.iodepth;
	fio_thread->iocq = (struct io_u**)calloc(fio_thread->iocq_size, sizeof(struct io_u *));
	assert(fio_thread->iocq != NULL);

	fio_thread->io_reactor = get_io_reactor();

	SPDK_NOTICELOG("Reactor %d is assigned for current thread io\n", fio_thread->io_reactor);

	TAILQ_INIT(&fio_thread->targets);

	return 0;
}

static void
spdk_fio_bdev_close_targets(void *arg)
{
	struct spdk_fio_thread *fio_thread = (struct spdk_fio_thread *)arg;
	struct spdk_fio_target *target, *tmp;

	TAILQ_FOREACH_SAFE(target, &fio_thread->targets, link, tmp) {
		TAILQ_REMOVE(&fio_thread->targets, target, link);
		spdk_put_io_channel(target->ch);
		spdk_bdev_close(target->desc);
		free(target);
	}
}

static void
spdk_fio_cleanup_thread(struct spdk_fio_thread *fio_thread)
{
	spdk_thread_send_msg(fio_thread->thread, spdk_fio_bdev_close_targets, fio_thread);

	while (spdk_fio_poll_thread(fio_thread) > 0) {}

	spdk_set_thread(fio_thread->thread);

	spdk_thread_exit(fio_thread->thread);
	free(fio_thread->iocq);
	free(fio_thread);
}


static pthread_t g_init_thread_id = 0;
static pthread_mutex_t g_init_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_init_cond;
static bool g_poll_loop = true;

static void fio_options_init(void);
static void *
spdk_init_thread_poll(void *arg)
{
	int				rc = 0;

	/* Create a dummy thread data for use on the initialization thread. */
	/* Parse the SPDK configuration file */
    intialize_fio_plugin_bdev(); 

	pthread_mutex_lock(&g_init_mtx);
	pthread_cond_signal(&g_init_cond);


	pthread_mutex_unlock(&g_init_mtx);

	pthread_exit(NULL);

	exit(rc);
	return NULL;   
}

#define MAX_CMD_STR     (256)
#define VOLUME_INDEX    (0)
#define SETUPPATH_INDEX (1)
#define MAX_INDEX       ((SETUPPATH_INDEX)+1)

static char *
eleminates_quotes(char *input_str)
{
	int last_index = strlen(input_str) - 1;
	if(input_str[last_index] == '\'' || input_str[last_index] == '\"')
	{
		input_str[last_index] = '\0';
	}		
	if(input_str[0] == '\'' || input_str[0] == '\"')
	{
		input_str = input_str + 1;
	}
	return input_str;
}    


static int
parse_filename(char ***parsed_str, struct thread_data *td)
{	
	char *parameter[MAX_INDEX]={"vol=", "setup_path="}, *ptr, *next, *original_filename;
	unsigned int i, index;
	struct fio_file *f;

	// This memory will be released in process termination. 
	*parsed_str = (char **)calloc(sizeof(char *), MAX_INDEX);
	for_each_file(td, f, i) {
		for(index = 0; index < MAX_INDEX; index++)
		{    
			original_filename = eleminates_quotes (f->file_name);
			ptr = strstr(original_filename, parameter[index]);
			if (ptr == NULL)
			{
				SPDK_ERRLOG("Please specify option of %s in filename.\n", parameter[index]);
				return -1;
			}
			next = ptr; 
			while(*next != ' ' && *next != '\0')
			{
				next++;
			}
			ptr = ptr + strlen(parameter[index]); 
			(*parsed_str)[index] = (char *)calloc(next - ptr + 1, 1); // + 1 means null pointer
			memcpy((*parsed_str)[index], ptr, next - ptr);
		}            
	}
	return 0;
}
static int
spdk_fio_init_env(struct thread_data *td)
{
    pthread_condattr_t attr;
    int rc = -1;
    char **parsed_string;
    char cmd[MAX_CMD_STR]="";

    if (pthread_condattr_init(&attr)) {
        SPDK_ERRLOG("Unable to initialize condition variable\n");
        return -1;
    }

    if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC)) {
        SPDK_ERRLOG("Unable to initialize condition variable\n");
        goto out;
    }

    if (pthread_cond_init(&g_init_cond, &attr)) {
        SPDK_ERRLOG("Unable to initialize condition variable\n");
        goto out;
    }

    /*
     * Spawn a thread to handle initialization operations and to poll things
     * like the admin queues periodically.
     */

    rc = pthread_create(&g_init_thread_id, NULL, &spdk_init_thread_poll, td->eo);
    if (rc != 0) {
        SPDK_ERRLOG("Unable to spawn thread to poll admin queue. It won't be polled.\n");
    }

    /* Wait for background thread to advance past the initialization */
        
    rc = parse_filename(&parsed_string, td);
    if (rc != 0) {
        SPDK_ERRLOG("Unable to parse filename parameter in fio.\n");
        goto out;
    }

    volume_name = parsed_string[VOLUME_INDEX];
    setup_path  = parsed_string[SETUPPATH_INDEX];

    sprintf(cmd, "cd %s && ./setup_bdev_volume.sh", setup_path);

    SPDK_NOTICELOG("Wait for ibofos initialization\n");
    sleep(5);
    rc = system(cmd);
    if (rc != 0)
    {
        SPDK_ERRLOG("Please indicate proper setupscript path.\n");
        goto out;
    }        
        
    pthread_mutex_lock(&g_init_mtx);
    pthread_cond_wait(&g_init_cond, &g_init_mtx);
    pthread_mutex_unlock(&g_init_mtx);
	
out:
    //pthread_condattr_destroy(&attr);
    return rc;
}

/* Called for each thread to fill in the 'real_file_size' member for
 * each file associated with this thread. This is called prior to
 * the init operation (spdk_fio_init()) below. This call will occur
 * on the initial start up thread if 'create_serialize' is true, or
 * on the thread actually associated with 'thread_data' if 'create_serialize'
 * is false.
 */
static int
spdk_fio_setup(struct thread_data *td)
{
	unsigned int i;
	struct fio_file *f;

    
	if (!td->o.use_thread) {
		SPDK_ERRLOG("must set thread=1 when using spdk plugin\n");
		return -1;
	}

	if (!g_spdk_env_initialized) {
		if (spdk_fio_init_env(td)) {
			SPDK_ERRLOG("failed to initialize\n");
			return -1;
		}

		g_spdk_env_initialized = true;
	}

    printf("### fio init completed ###\n");
	for_each_file(td, f, i) {
		struct spdk_bdev *bdev;

		bdev = spdk_bdev_get_by_name(volume_name);
		if (!bdev) {
			SPDK_ERRLOG("Unable to find bdev with name %s\n", f->file_name);
			return -1;
		}

		f->real_file_size = spdk_bdev_get_num_blocks(bdev) *
				    spdk_bdev_get_block_size(bdev);

	}

	return 0;
}

static void
spdk_fio_bdev_open(void *arg)
{
	struct thread_data *td = (struct thread_data *)arg;
	struct spdk_fio_thread *fio_thread;
	unsigned int i;
	struct fio_file *f;
	int rc;

	fio_thread = (struct spdk_fio_thread *)td->io_ops_data;

	for_each_file(td, f, i) {
		struct spdk_fio_target *target;

		target = (spdk_fio_target *)calloc(1, sizeof(*target));
		if (!target) {
			SPDK_ERRLOG("Unable to allocate memory for I/O target.\n");
			fio_thread->failed = true;
			return;
		}

		target->bdev = spdk_bdev_get_by_name(volume_name);
		if (!target->bdev) {
			SPDK_ERRLOG("Unable to find bdev with name %s\n", f->file_name);
			free(target);
			fio_thread->failed = true;
			return;
		}

		target->bdev->internal.claim_module = NULL;

		rc = spdk_bdev_open(target->bdev, true, NULL, NULL, &target->desc);
		if (rc) {
			SPDK_ERRLOG("Unable to open bdev %s\n", f->file_name);
			free(target);
			fio_thread->failed = true;
			return;
		}

		target->ch = spdk_bdev_get_io_channel(target->desc);
		if (!target->ch) {
			SPDK_ERRLOG("Unable to get I/O channel for bdev.\n");
			spdk_bdev_close(target->desc);
			free(target);
			fio_thread->failed = true;
			return;
		}

		f->engine_data = target;

		TAILQ_INSERT_TAIL(&fio_thread->targets, target, link);
	}
}

/* Called for each thread, on that thread, shortly after the thread
 * starts.
 */
static int
spdk_fio_init(struct thread_data *td)
{
	struct spdk_fio_thread *fio_thread;

	spdk_fio_init_thread(td);
	fio_thread = (struct spdk_fio_thread *)td->io_ops_data;
	fio_thread->failed = false;

	spdk_thread_send_msg(fio_thread->thread, spdk_fio_bdev_open, td);

	while (spdk_fio_poll_thread(fio_thread) > 0) {}

	if (fio_thread->failed) {
		return -1;
	}
	return 0;
}

static void
spdk_fio_cleanup(struct thread_data *td)
{
	struct spdk_fio_thread *fio_thread = (struct spdk_fio_thread *)td->io_ops_data;

	spdk_fio_cleanup_thread(fio_thread);
	td->io_ops_data = NULL;
}

static int
spdk_fio_open(struct thread_data *td, struct fio_file *f)
{

	return 0;
}

static int
spdk_fio_close(struct thread_data *td, struct fio_file *f)
{
	return 0;
}

static int
spdk_fio_iomem_alloc(struct thread_data *td, size_t total_mem)
{
	td->orig_buffer = (char *)spdk_dma_zmalloc(total_mem, 0x1000, NULL);
	return td->orig_buffer == NULL;
}

static void
spdk_fio_iomem_free(struct thread_data *td)
{
	spdk_dma_free(td->orig_buffer);
}

static int
spdk_fio_io_u_init(struct thread_data *td, struct io_u *io_u)
{
	struct spdk_fio_request	*fio_req;

	fio_req = (spdk_fio_request *)calloc(1, sizeof(*fio_req));
	if (fio_req == NULL) {
		return 1;
	}
	fio_req->io = io_u;
	fio_req->td = td;

	io_u->engine_data = fio_req;

	return 0;
}

static void
spdk_fio_io_u_free(struct thread_data *td, struct io_u *io_u)
{
	struct spdk_fio_request *fio_req = (struct spdk_fio_request *)io_u->engine_data;

	if (fio_req) {
		assert(fio_req->io == io_u);
		free(fio_req);
		io_u->engine_data = NULL;
	}
}

//this will be only used by fio thread. 
volatile int g_pending_cnt = 0;



static void 
spdk_fio_completion_cb(void *arg)
{
    struct spdk_fio_request		*fio_req = (struct spdk_fio_request *)arg;
	struct thread_data		*td = fio_req->td;
	struct spdk_fio_thread		*fio_thread = (struct spdk_fio_thread *)td->io_ops_data;

	fio_req->io->error = 0;
	fio_thread->iocq[fio_thread->iocq_count++] = fio_req->io;
}

static void
spdk_fio_completion_cb_in_reactor(struct spdk_bdev_io *bdev_io,
		       bool success,
		       void *cb_arg)
{
	struct spdk_fio_request		*fio_req = (struct spdk_fio_request *)cb_arg;
	struct thread_data		*td = fio_req->td;
	struct spdk_fio_thread		*fio_thread = (struct spdk_fio_thread *)td->io_ops_data;
    fio_req->bdev_io = bdev_io;
	spdk_bdev_free_io(fio_req->bdev_io);
	spdk_thread_send_msg(fio_thread->thread, spdk_fio_completion_cb, cb_arg);
}

#if FIO_IOOPS_VERSION >= 24
typedef enum fio_q_status fio_q_status_t;
#else
typedef int fio_q_status_t;
#endif

void 
_spdk_fio_queue(void *arg1, void *arg2)
{
    struct thread_data *td = (struct thread_data *)arg1;
    struct io_u *io_u = (struct io_u *)arg2;
    	
    int rc = 1;
	struct spdk_fio_request	*fio_req = (struct spdk_fio_request *)io_u->engine_data;
	struct spdk_fio_target *target = (struct spdk_fio_target *)io_u->file->engine_data;


	assert(fio_req->td == td);

	if (!target) {
		SPDK_ERRLOG("Unable to look up correct I/O target.\n");
		fio_req->io->error = ENODEV;
	}

	switch (io_u->ddir) {
	case DDIR_READ:
		rc = spdk_bdev_read(target->desc, target->ch,
				    io_u->buf, io_u->offset, io_u->xfer_buflen,
				    spdk_fio_completion_cb_in_reactor, fio_req);
		break;
	case DDIR_WRITE:
		rc = spdk_bdev_write(target->desc, target->ch,
				     io_u->buf, io_u->offset, io_u->xfer_buflen,
				     spdk_fio_completion_cb_in_reactor, fio_req);
		break;
	case DDIR_TRIM:
		rc = spdk_bdev_unmap(target->desc, target->ch,
				     io_u->offset, io_u->xfer_buflen,
				     spdk_fio_completion_cb_in_reactor, fio_req);
		break;
	default:
		assert(false);
		break;
	}

	if (rc != 0) {
		fio_req->io->error = abs(rc);
	}	
}

static fio_q_status_t
spdk_fio_queue(struct thread_data *td, struct io_u *io_u)
{
	struct spdk_fio_thread *fio_thread = (struct spdk_fio_thread *)td->io_ops_data;
	struct spdk_event* e = spdk_event_allocate(fio_thread->io_reactor, _spdk_fio_queue, td, io_u);
	assert (e != NULL);
	spdk_event_call(e);

	return FIO_Q_QUEUED;
}

static struct io_u *
spdk_fio_event(struct thread_data *td, int event)
{
	struct spdk_fio_thread *fio_thread = (struct spdk_fio_thread *)td->io_ops_data;
	assert(event >= 0);
	assert((unsigned)event < fio_thread->iocq_count);
	return fio_thread->iocq[event];
}

static size_t
spdk_fio_poll_thread(struct spdk_fio_thread *fio_thread)
{
	return spdk_thread_poll(fio_thread->thread, 0, 0);
}

static int
spdk_fio_getevents(struct thread_data *td, unsigned int min,
		   unsigned int max, const struct timespec *t)
{
	struct spdk_fio_thread *fio_thread = (struct spdk_fio_thread *)td->io_ops_data;
	struct timespec t0, t1;
	uint64_t timeout = 0;
    unsigned int retVal = 0;
	if (t) {
		timeout = t->tv_sec * SPDK_SEC_TO_NSEC + t->tv_nsec;
		clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
	}
    fio_thread->iocq_count = 0;

	for (;;) {
        spdk_fio_poll_thread(fio_thread);
        retVal = fio_thread->iocq_count;
		if (retVal >= min) {
			return retVal;
		}

		if (t) {
			clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
			uint64_t elapse = ((t1.tv_sec - t0.tv_sec) * SPDK_SEC_TO_NSEC)
					  + t1.tv_nsec - t0.tv_nsec;
			if (elapse > timeout) {
				break;
			}
		}
    }
			
    return fio_thread->iocq_count;
}

static int
spdk_fio_invalidate(struct thread_data *td, struct fio_file *f)
{
	/* TODO: This should probably send a flush to the device, but for now just return successful. */
	return 0;
}

struct ioengine_ops ioengine;
static void fio_options_init(void)
{
	options[0].name		= "spdk_conf";
	options[0].lname		= "SPDK configuration file";
    options[0].type		= FIO_OPT_STR_STORE;
	options[0].off1		= offsetof(struct spdk_fio_options, conf);
	options[0].help		= "A SPDK configuration file";
	options[0].category	= FIO_OPT_C_ENGINE;
	options[0].group		= FIO_OPT_G_INVALID;

	options[1].name		= "spdk_mem";
	options[1].lname		= "SPDK memory in MB";
	options[1].type		= FIO_OPT_INT;
	options[1].off1		= offsetof(struct spdk_fio_options, mem_mb);
	options[1].help		= "Amount of memory in MB to allocate for SPDK";
	options[1].category	= FIO_OPT_C_ENGINE;
	options[1].group		= FIO_OPT_G_INVALID;

	options[2].name		= "spdk_single_seg";
	options[2].lname		= "SPDK switch to create just a single hugetlbfs file";
	options[2].type		= FIO_OPT_BOOL;
	options[2].off1		= offsetof(struct spdk_fio_options, mem_single_seg);
	options[2].help		= "If set to 1, SPDK will use just a single hugetlbfs file";
	options[2].category	= FIO_OPT_C_ENGINE;
	options[2].group		= FIO_OPT_G_INVALID;
	options[3].name		= NULL;


}

static void fio_init spdk_fio_register(void)
{

    fio_options_init();
    ioengine.name			= "spdk_bdev";
	ioengine.version		= FIO_IOOPS_VERSION;
	ioengine.flags			= FIO_RAWIO | FIO_NOEXTEND | FIO_NODISKUTIL | FIO_MEMALIGN;
	ioengine.setup			= spdk_fio_setup;
	ioengine.init			= spdk_fio_init;
	/* .prep		= unused, */
	ioengine.queue			= spdk_fio_queue;
	/* .commit		= unused, */
	ioengine.getevents		= spdk_fio_getevents;
	ioengine.event			= spdk_fio_event;
	/* .errdetails		= unused, */
	/* .cancel		= unused, */
	ioengine.cleanup		= spdk_fio_cleanup;
	ioengine.open_file		= spdk_fio_open;
	ioengine.close_file		= spdk_fio_close;
	ioengine.invalidate		= spdk_fio_invalidate;
	/* .unlink_file		= unused, */
	/* .get_file_size	= unused, */
	/* .terminate		= unused, */
	ioengine.iomem_alloc		= spdk_fio_iomem_alloc;
	ioengine.iomem_free		= spdk_fio_iomem_free;
	ioengine.io_u_init		= spdk_fio_io_u_init;
	ioengine.io_u_free		= spdk_fio_io_u_free;
	ioengine.option_struct_size	= sizeof(struct spdk_fio_options);

	ioengine.options		= options;
  	register_ioengine(&ioengine);
}

static void
spdk_fio_finish_env(void)
{
	pthread_mutex_lock(&g_init_mtx);
	g_poll_loop = false;
	pthread_cond_signal(&g_init_cond);
	pthread_mutex_unlock(&g_init_mtx);
	pthread_join(g_init_thread_id, NULL);

	spdk_thread_lib_fini();
}

static void fio_exit spdk_fio_unregister(void)
{
	if (g_spdk_env_initialized) {
		spdk_fio_finish_env();
		g_spdk_env_initialized = false;
	}

	unregister_ioengine(&ioengine);
}
}
