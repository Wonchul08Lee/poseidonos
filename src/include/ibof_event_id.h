/*
 *   BSD LICENSE
 *   Copyright (c) 2021 Samsung Electronics Corporation
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

#ifndef __IBOF_EVENT_ID_H__
#define __IBOF_EVENT_ID_H__

#define EID(X) ((int)IBOF_EVENT_ID::X)

enum class IBOF_EVENT_ID
{
    SUCCESS = 0,
    IBOFOS_STARTED = 1000,
    IBOFOS_TERMINATED = 1001,
    SERVER_READY = 1002,
    CLIENT_CONNECTED = 1003,
    CLIENT_DISCONNECTED = 1004,
    MSG_RECEIVED = 1005,
    MSG_SENT = 1006,
    SERVER_TRY_EXIT = 1010,
    SERVER_THREAD_JOINED = 1011,
    REUSE_ADDR_ENABLED = 1012,
    REUSE_ADDR_FAILED = 1013,
    SOCK_CREATE_FAILED = 1014,
    SOCK_BIND_FAILED = 1015,
    SOCK_LISTEN_FAILED = 1016,
    EPOLL_CREATE_FAILED = 1017,
    SOCK_ACCEPT_FAILED = 1018,
    MAX_CLIENT_ERROR = 1019,
    MSG_SEND_FAILED = 1020,
    MSG_RECEIVE_EXCEPTION = 1021,
    TIMED_OUT = 1022,
    INVALID_PARAM = 1023,
    POS_BUSY = 1030,
    SYSTEM_RECOVERY = 1234,
    STATE_CONTEXT_UPDATED = 1101,
    STATE_CHANGED = 1102,
    VOL_CREATED = 2000,
    VOL_DELETED = 2001,
    VOL_MOUNTED = 2002,
    VOL_UNMOUNTED = 2003,
    VOL_ADDED = 2004,
    VOL_REMOVED = 2005,
    VOL_NOT_EXIST = 2010,
    VOL_CNT_EXCEEDED = 2011,
    VOL_SIZE_EXCEEDED = 2012,
    NAME_TOO_SHORT = 2020,
    NAME_TOO_LONG = 2021,
    NAME_DUPLICATED = 2022,
    NAME_NOT_ALLOWED = 2023,
    SIZE_TOO_BIG = 2030,
    SIZE_TOO_SMALL = 2031,
    SIZE_NOT_ALIGNED = 2032,
    VOL_ALD_MOUNTED = 2040,
    VOL_ALD_UNMOUNTED = 2041,
    DEL_MOUNTED_VOL = 2050,
    META_CREATE_FAIL = 2060,
    META_OPEN_FAIL = 2061,
    META_READ_FAIL = 2062,
    META_WRITE_FAIL = 2063,
    META_CONTENT_BROKEN = 2064,
    MEM_ALLOC_FAIL = 2070,
    INVALID_INDEX = 2071,
    VOLID_ALLOC_FAIL = 2072,
    VOL_SAMEID_EXIST = 2073,
    VOL_UNEXPECTED_PENDING_IO_COUNT = 2074,
    OUT_OF_QOS_RANGE = 2080,
    ARRAY_NOT_MOUNTED = 2090,
    SYSTEM_FAULT = 2091,
    DONE_WITH_ERROR = 2092,
    SUBSYSTEM_NOT_CREATED = 2100,
    VOL_ALD_SET_SUBNQN = 2101,
    CANNOT_EXTEND_NSID = 2102,
    // --------------Mbr (2300)-----------------------
    MBR_START = 2300,
    MBR_ALLOCATE_MEMORY = MBR_START,
    MBR_CREATE_AND_WRITE,
    MBR_ADD_AND_WRITE,
    MBR_WRITE_DONE,
    MBR_READ_DONE,
    MBR_PARITY_CHECK,
    MBR_SYSTEM_UUID_CHECK,
    MBR_DEVICE_NOT_FOUND = 2310,
    MBR_PARITY_CHECK_FAILED,
    MBR_SYSTEM_UUID_CHECK_FAILED,
    MBR_DATA_NOT_FOUND,
    MBR_KEY_NOT_FOUND,
    MBR_ALLOCATE_MEMORY_FAILED = 2330,
    MBR_DATA_SIZE_ERROR,
    MBR_FORMAT_ERROR,
    MBR_GET_SYSTEM_UUID_FAILED,
    MBR_END,
    MBR_COUNT = MBR_END - MBR_START,

    // --------------Array (2500)-----------------------
    ARRAY_START = 2500,
    ARRAY_STATE_ONLINE = ARRAY_START,
    ARRAY_STATE_OFFLINE,
    ARRAY_STATE_EXIST,
    ARRAY_STATE_NOT_EXIST,
    ARRAY_DEVICE_NOT_FOUND,
    ARRAY_DEVICE_COUNT_ERROR,
    ARRAY_NVM_CAPACITY_ERROR,
    ARRAY_SSD_CAPACITY_ERROR,
    ARRAY_SSD_SAME_CAPACITY_ERROR,
    ARRAY_MBR_READ_ERROR,
    ARRAY_MBR_WRITE_ERROR,
    ARRAY_FT_METHOD_ERROR,
    ARRAY_WRONG_FT_METHOD,
    ARRAY_PARTITION_CREATION_ERROR,
    ARRAY_PARTITION_LOAD_ERROR,
    ARRAY_WRONG_EVENT_ID,
    ARRAY_INVALID_ADDRESS_ERROR,
    ARRAY_BROKEN_ERROR,
    ARRAY_DEVICE_TYPE_ERROR,
    ARRAY_PARTITION_TRIM,

    ARRAY_DEVICE_DETACHED = 2520,
    ARRAY_NO_REMAINING_SPARE = 2521,
    ARRAY_WRONG_NAME = 2522,
    ARRAY_NEED_MOUNT = 2523,

    ARRAY_DEVICE_ADDED = 2530,
    ARRAY_DEVICE_REMOVED,
    ARRAY_DEVICE_ADD_FAIL,
    ARRAY_DEVICE_REMOVE_FAIL,
    ARRAY_DEVICE_CLEARED,
    ARRAY_DEVICE_REPLACED,

    ARRAY_DEVICE_WRONG_NAME = 2536,
    ARRAY_DEVICE_ALREADY_ADDED,

    ARRAY_STATE_CHANGED = 2550,
    ARRAY_STATE_EXIST_NORMAL,
    ARRAY_STATE_EXIST_DEGRADED,
    ARRAY_STATE_EXIST_MISSING,
    ARRAY_STATE_EXIST_BROKEN,
    ARRAY_STATE_NORMAL,
    ARRAY_STATE_DEGRADED,
    ARRAY_STATE_REBUILDING,
    ARRAY_STATE_STOP,
    ARRAY_STATE_CHANGE_ERROR,

    ARRAY_DEBUG_MSG = 2600,

    ARRAY_END = 2799,
    ARRAY_COUNT = ARRAY_END - ARRAY_START,

    // --------------Rebuilder (2800)----------------
    REBUILD_START = 2800,
    REBUILD_DEBUG_MSG = REBUILD_START,
    REBUILD_STOPPED = 2802,
    REBUILD_FAILED = 2803,
    REBUILD_PROGRESS = 2804,
    REBUILD_PROGRESS_DETAIL = 2805,
    REBUILD_STRIPE_LOCK = 2806,
    REBUILD_STRIPE_UNLOCK = 2807,
    REBUILD_RESULT_PASS = 2810,
    REBUILD_RESULT_FAILED = 2811,
    REBUILD_RESULT_CANCELLED = 2812,
    REBUILD_TRIGGER_FAIL,
    REBUILD_END,

    // --------------CONFIG (2900)----------------
    CONFIG_START = 2900,
    CONFIG_FILE_READ_DONE = CONFIG_START,

    CONFIG_REQUEST_CONFIG_TYPE_ERROR = 2910,
    CONFIG_REQUEST_KEY_ERROR,
    CONFIG_REQUEST_MODULE_ERROR,

    CONFIG_FILE_OPEN_FAIL = 2920,
    CONFIG_FILE_SIZE_ERROR,
    CONFIG_FILE_READ_ERROR,
    CONFIG_FILE_FORMAT_ERROR,
    CONFIG_JSON_DOC_IS_NOT_OBJECT,
    CONFIG_VALUE_TYPE_ERROR,

    CONFIG_END,
    CONFIG_COUNT = CONFIG_END - CONFIG_START,

    // --------------Journal manager (3000)----------------
    JOURNAL_START = 3000,
    JOURNAL_MANAGER_NOT_INITIALIZED = JOURNAL_START,
    JOURNAL_ENABLED,
    JOURNAL_DISABLED,
    JOURNAL_LOG_BUFFER_CREATED,
    JOURNAL_LOG_BUFFER_LOADED,
    JOURNAL_LOG_BUFFER_RESET,
    JOURNAL_LOG_BUFFER_RESET_FAILED,
    JOURNAL_LOG_BUFFER_CREATE_FAILED,
    JOURNAL_LOG_BUFFER_OPEN_FAILED,
    JOURNAL_LOG_BUFFER_CLOSE_FAILED,
    JOURNAL_LOG_BUFFER_READ_FAILED,
    JOURNAL_LOG_BUFFER_WRITE_FAILED,
    JOURNAL_LOG_BUFFER_STATUS_UPDATE_FAILED,
    JOURNAL_INVALID_LOG_FOUND,
    JOURNAL_LOG_WRITTEN,
    JOURNAL_LOG_WRITE_FAILED,
    ADD_TO_JOURNAL_WAITING_LIST,
    JOURNAL_CALLBACK_FAILED,
    JOURNAL_CANNOT_START_REPLAY,
    JOURNAL_REPLAY_STARTED,
    JOURNAL_REPLAY_STATUS,
    JOURNAL_REPLAY_STOPPED,
    JOURNAL_REPLAY_FAILED,
    JOURNAL_REPLAY_STRIPE,
    JOURNAL_REPLAY_STRIPE_FLUSHED,
    JOURNAL_REPLAY_SSD_LSID,
    JOURNAL_REPLAY_WB_TAIL,
    JOURNAL_REPLAY_VOLUME_EVENT,
    JOURNAL_REPLAY_ACTIVE_STRIPE,
    JOURNAL_REPLAY_STRIPE_FLUSH_FAILED,
    JOURNAL_NO_LOG_BUFFER_AVAILABLE,
    JOURNAL_FLUSH_LOG_GROUP,
    JOURNAL_LOG_GROUP_FULL,
    JOURNAL_CHECKPOINT_STARTED,
    JOURNAL_CHECKPOINT_STATUS,
    JOURNAL_CHECKPOINT_COMPLETED,
    JOURNAL_CHECKPOINT_FAILED,
    JOURNAL_COMPLETE_FAILED,
    JOURNAL_LOG_DELETE_FAILED,
    JOURNAL_VOLUME_DELETE_LOG_WRITE_STARTED,
    JOURNAL_VOLUME_DELETE_LOG_WRITTEN,
    JOURNAL_DEBUG,
    JOURNAL_END,
    JOURNAL_COUNT = JOURNAL_END - JOURNAL_START,

    // --------------Mapper (3100)----------------
    MAPPER_START = 3100,

    MAPPER_SUCCESS = MAPPER_START,
    MAP_FLUSH_STARTED,
    MAP_FLUSH_IN_PROGRESS,
    MAP_FLUSH_ONGOING,
    MAP_FLUSH_COMPLETED,

    MAP_LOAD_STARTED,
    MAP_LOAD_ONGOING,
    MAP_LOAD_COMPLETED,
    MPAGE_NULLPTR,
    MPAGE_ALREADY_EXIST,

    MPAGE_MEMORY_ALLOC_FAILURE,
    VSAMAP_NULL_PTR,
    VSAMAP_SET_FAILURE,
    STRIPEMAP_SET_FAILURE,
    MAPCONTENT_HEADER_NOT_INITIALIZED,

    INVALID_VOLUME_ID,
    REVMAP_GET_MFS_ALIGNED_IOSIZE_FAILURE,
    REVMAP_PACK_ALREADY_LINKED,
    REVMAP_NOT_LINKED_PACK,
    REVMAP_FILE_SIZE,
    REVMAP_VOLUME_ID_FOUND,
    REVMAP_VOLUME_ID_NOT_FOUND,

    REVMAP_MFS_IO_ERROR,
    MAP_UPDATE_HANDLER_EVENT_ALLOCATE_FAIL,
    TRIED_TO_LOAD_WITHOUT_MFSFILE,
    NO_BLOCKMAP_MFS_FILE,
    VSAMAP_HEADER_LOAD_FAILURE,

    VSAMAP_LOAD_FAILURE,
    VSAMAP_NOT_LOADED,
    VSAMAP_UNMOUNT_FAILURE,
    VSAMAP_INVALIDATE_ALLBLKS_FAILURE,
    VSAMAP_UNLOAD_FAILURE,

    VSAMAP_STORE_FAILURE,
    STRIPEMAP_STORE_FAILURE,
    MFS_SYNCIO_ERROR,
    MFS_ASYNCIO_ERROR,
    VSAMAP_NOT_ACCESSIBLE,

    WRONG_MAP_ID,
    MAPPER_FAILED,
    MAPPER_END,
    MAPPER_COUNT = MAPPER_END - MAPPER_START,

    // --------------Allocator (3150)----------------
    ALLOCATOR_START = 3150,

    ALLOCATOR_META_ARCHIVE_STORE = ALLOCATOR_START,
    ALLOCATOR_META_ARCHIVE_LOAD,
    ALLOCATOR_META_ARCHIVE_STORE_REBUILD_SEGMENT,
    ALLOCATOR_META_ARCHIVE_LOAD_REBUILD_SEGMENT,
    ALLOCATOR_META_ARCHIVE_FLUSH_IN_PROGRESS,

    ALLOCATOR_MAKE_REBUILD_TARGET,
    ALLOCATOR_MAKE_REBUILD_TARGET_FAILURE,
    ALLOCATOR_TARGET_SEGMENT_FREED,
    ALLOCATOR_REBUILD_TARGET_SET_NOT_EMPTY,
    ALLOCATOR_REBUILD_TARGET_SET_EMPTY,

    ALLOCATOR_NO_FREE_WB_STRIPE,
    ALLOCATOR_REBUILDING_SEGMENT,
    ALLOCATOR_REBUILT_SEGMENT,
    ALLOCATOR_NO_FREE_SEGMENT,
    ALLOCATOR_WRONG_STRIPE_REFERENCE_COUNT,

    ALLOCATOR_CANNOT_ALLOCATE_USER_BLOCK,
    ALLOCATOR_CANNOT_ALLOCATE_STRIPE,
    ALLOCATOR_CANNOT_LINK_REVERSE_MAP,
    ALLOCATOR_STRIPE_WITHOUT_REVERSEMAP,
    ALLOCATOR_RECONSTRUCT_STRIPE,
    ALLOCATOR_REPLAYED_STRIPE_IS_FULL,
    ALLOCATOR_REPLAY_SEGMENT_STATUS,
    ALLOCATOR_TRIGGER_FLUSH,
    REVMAP_RECONSTRUCT_FOUND_RBA,

    REVMAP_RECONSTRUCT_NOT_FOUND_RBA,
    GET_VOLUMESIZE_FAILURE,
    WRONG_BLOCK_COUNT,
    PICKUP_ACTIVE_STRIPE,

    ALLOCATOR_END,
    ALLOCATOR_COUNT = ALLOCATOR_END - ALLOCATOR_START,

    // --------------Garbage collector (3400)----------------
    GC_START = 3400,
    GC_TRIGGERED = GC_START,
    GC_STARTED = 3401,
    GC_DONE = 3402,
    GC_CANNOT_START = 3403,
    GC_VICTIM_STRIPE_CONSTRUCTOR = 3410,
    GC_LOAD_REVERSE_MAP = 3411,
    GC_LOAD_VALID_BLOCKS = 3412,
    GC_GET_UNMAP_VSA = 3413,
    GC_GET_UNMAP_LSA = 3414,
    GC_GET_VICTIM_SEGMENT = 3415,
    GC_GET_VALID_BLOCKS = 3416,
    GC_GET_VALID_BLOCK_INFO = 3417,
    GC_COPY_COMPLETE = 3418,
    GC_GET_BUFFER_FAILED = 3419,

    // --------------Meta filesystem (4000)----------------
    // Debug, Info, Warning, Error, Critical
    MFS_START = 4000,
    MFS_DEBUG_START = MFS_START,
    MFS_DEBUG_MESSAGE = MFS_DEBUG_START,
    MFS_INFO_START = 4100,
    MFS_INFO_MESSAGE = MFS_INFO_START,
    MFS_WARNING_START = 4200,
    MFS_WARNING_INIT_AGAIN = MFS_WARNING_START,
    MFS_SYSTEM_MOUNT_AGAIN,
    MFS_SYSTEM_UNMOUNT_AGAIN,
    MFS_MSG_ENQUEUE_FAILED,
    MFS_COMPACTION_FAILED,

    MFS_ERROR_START = 4300,
    MFS_ERROR_MOUNTED = MFS_ERROR_START,
    MFS_ERROR_UNMOUNTED,
    MFS_ERROR_MESSAGE,
    MFS_INVALID_PARAMETER,
    MFS_MODULE_NOT_READY,
    MFS_MODULE_ALREADY_READY,
    MFS_MODULE_INIT_FAILED,
    MFS_MODULE_BRINGUP_FAILED,
    MFS_MODULE_NO_MEDIA,
    MFS_SYSTEM_OPEN_FAILED,
    MFS_INVALID_INFORMATION,

    MFS_FILE_CREATE_FAILED,
    MFS_FILE_NOT_FOUND,
    MFS_FILE_NOT_OPEND,
    MFS_FILE_OPEN_FAILED,
    MFS_FILE_OPEN_REPETITIONARY,
    MFS_FILE_CLOSE_FAILED,
    MFS_FILE_FORMAT_FAILED,
    MFS_FILE_DELETE_FAILED,
    MFS_FILE_ALREADY_LOCKED,
    MFS_FILE_READ_FAILED,
    MFS_FILE_WRITE_FAILED,
    MFS_MEDIA_WROTE_SIZE_NOT_MATCHED,
    MFS_MEDIA_SEEK_FAILED,
    MFS_MEDIA_READ_FAILED,
    MFS_MEDIA_WRITE_FAILED,
    MFS_FILE_TRIM_FAILED,

    MFS_MEDIA_MOUNT_FAILED,
    MFS_MEDIA_UNMOUNT_FAILED,
    MFS_MEDIA_NULL,
    MFS_META_STORAGE_CREATE_FAILED,
    MFS_META_STORAGE_CLOSE_FAILED,
    MFS_META_STORAGE_NOT_READY,

    MFS_META_VOLUME_CREATE_FAILED,
    MFS_META_VOLUME_OPEN_FAILED,
    MFS_META_VOLUME_CLOSE_FAILED,
    MFS_META_VOLUME_CLOSE_FAILED_DUE_TO_ACTIVE_FILE,
    MFS_META_VOLUME_NOT_ENOUGH_SPACE,
    MFS_META_VOLUME_ALMOST_FULL,
    MFS_META_VOLUME_ALREADY_CLOSED,
    MFS_META_VOLUME_CATALOG_INVALID,
    MFS_META_SAVE_FAILED,
    MFS_META_LOAD_FAILED,
    MFS_IO_FAILED_DUE_TO_ENQUEUE_FAILED,
    MFS_IO_FAILED_DUE_TO_STOP_STATE,
    MFS_IO_FAILED_DUE_TO_ERROR,
    MFS_QUEUE_POP_FAILED,
    MFS_QUEUE_PUSH_FAILED,
    MFS_MAX_FILE_SIZE_NOT_SET,
    MFS_CRITICAL_START = 4400,
    MFS_RECOVERY_CATALOG_FAILED = MFS_CRITICAL_START,
    MFS_TEST_FAILED,
    MFS_END,
    MFS_COUNT = MFS_END - MFS_START,
    // Event
    VOLUME_EVENT = 4500,
    DEVICE_DETACH_EVENT = 4510,
    STATE_EVENT = 4520,

    LOGGER_FILTER_POLICY_DECODE_FAIL = 4580,
    LOGGER_FILTER_POLICY_FILE_NOT_FOUND = 4581,
    LOGGER_SET_LEVEL_FAILED = 4582,

    // --------------QOS Manager (4600)----------------
    QOS_SET_EVENT_POLICY = 4600,
    // --------------IO Path nvmf (5000)----------------
    IONVMF_START = 5000,

    IONVMF_1H = IONVMF_START,
    IONVMF_2H,
    IONVMF_3H,
    IONVMF_4H,
    IONVMF_5H,
    IONVMF_6H,
    IONVMF_7H,
    IONVMF_8H,
    IONVMF_NAMESPACE_ATTACH_FAILED,
    // IONVMF_9H_FFH,    // Reserved

    IONVMF_END,
    IONVMF_COUNT = IONVMF_END - IONVMF_START,

    // --------------IO Path Frontend (5100)----------------
    IOFRONTEND_START = 5100,

    AFTMGR_CPU_COUNT_NOT_ENOUGH = IOFRONTEND_START,
    AFTMGR_DISABLED_CORE,
    AFTMGR_FAIL_TO_FIND_MASTER_REACTOR,
    AFTMGR_FAIL_TO_ALLOCATE_ALL_CPU,
    AFTMGR_FAIL_TO_OVERLAP_MASK,
    AFTMGR_FAIL_TO_PARSING_ERROR,
    AFTMGR_NO_EVENT_WORKER_ALLOCATED,
    AFTMGR_NO_USE_CONFIG,
    AFTMGR_USE_CONFIG,

    AIO_CONTEXT_NOT_FOUND,
    AIO_FAIL_TO_ALLOCATE_EVENT,
    AIO_FAIL_TO_ALLOCATE_MEMORY,
    AIO_INVALID_AIO_CONTEXT,
    AIO_INVALID_AIO_PRIVATE,
    AIO_INVALID_BDEV_IO,
    AIO_INVALID_UBIO,
    AIO_IO_FROM_INVALID_THREAD,
    AIO_DEBUG_COMPLETION,
    AIO_DEBUG_SUBMISSION,
    AIO_FLUSH_START,
    AIO_FLUSH_END,

    BLKALGN_INVALID_UBIO,

    BLKHDLR_FAIL_TO_ALLOCATE_EVENT,
    BLKHDLR_WRONG_IO_DIRECTION,

    CALLBACK_INVALID_CALLEE,
    CALLBACK_INVALID_COUNT,
    CALLBACK_TIMEOUT,

    EVENTFRAMEWORK_FAIL_TO_ALLOCATE_EVENT,
    EVENTFRAMEWORK_INVALID_EVENT,
    EVENTSCHEDULER_NOT_MATCH_WORKER_COUNT,

    EVTARG_WRONG_ARGUMENT_ACCESS,
    EVTARG_WRONG_ARGUMENT_ADD,

    EVTQ_INVALID_EVENT,

    EVTSCHDLR_INVALID_WORKER_ID,

    FLUSHREAD_DEBUG_SUBMIT,
    FLUSHREAD_FAIL_TO_ALLOCATE_MEMORY,
    FLUSH_DEBUG_SUBMIT,
    FLUSH_DEBUG_COMPLETION,

    FREEBUFPOOL_FAIL_TO_ALLOCATE_MEMORY,

    IOAT_CONFIG_INVALID,

    IOATAPI_FAIL_TO_FINALIZE,
    IOATAPI_FAIL_TO_INITIALIZE,
    IOATAPI_FAIL_TO_SUBMIT_COPY,
    IOATAPI_DISABLED,
    IOATAPI_ENABLED,

    IODISPATCHER_INVALID_CPU_INDEX,
    IODISPATCHER_INVALID_PARM,

    MERGER_INVALID_SPLIT_REQUESTED,

    RBAMGR_WRONG_VOLUME_ID,

    RDCMP_INVALID_ORIGIN_UBIO,
    RDCMP_INVALID_UBIO,
    RDCMP_READ_FAIL,

    RDHDLR_INVALID_UBIO,
    RDHDLR_READ_DURING_REBUILD,

    SCHEDAPI_COMPLETION_POLLING_FAIL,
    SCHEDAPI_NULL_COMMAND,
    SCHEDAPI_SUBMISSION_FAIL,
    SCHEDAPI_WRONG_BUFFER,

    STRIPE_INVALID_VOLUME_ID,

    REF_COUNT_RAISE_FAIL,
    TRANSLATE_CONVERT_FAIL,
    TRANSLATE_CONVERT_FAIL_IN_SYSTEM_STOP,

    TRSLTR_INVALID_BLOCK_INDEX,
    TRSLTR_WRONG_ACCESS,
    TRSLTR_WRONG_VOLUME_ID,

    VOLUMEIO_DEBUG_SUBMIT,

    UBIO_DEBUG_CHECK_VALID,
    UBIO_DEBUG_COMPLETE,
    UBIO_ALREADY_SYNC_DONE,
    UBIO_CALLBACK_EVENT_ALREADY_SET,
    UBIO_DUPLICATE_IO_ABSTRACTION,
    UBIO_FAIL_TO_ALLOCATE_MEMORY,
    UBIO_FREE_UNALLOWED_BUFFER,
    UBIO_INVALID_GC_PROGRESS,
    UBIO_INVALID_IBOF_IO,
    UBIO_INVALID_IO_STATE,
    UBIO_INVALID_LSID,
    UBIO_INVALID_ORIGIN_UBIO,
    UBIO_INVALID_ORIGINAL_CORE,
    UBIO_INVALID_PBA,
    UBIO_INVALID_RBA,
    UBIO_INVALID_UBIO_HANDLER,
    UBIO_INVALID_VOLUME_ID,
    UBIO_INVALID_VSA,
    UBIO_NO_COMPLETION_FOR_FRONT_END_EVENT,
    UBIO_REMAINING_COUNT_ERROR,
    UBIO_REQUEST_NULL_BUFFER,
    UBIO_REQUEST_OUT_RANGE,
    UBIO_SYNC_FLAG_NOT_SET,
    UBIO_WRONG_SPLIT_SIZE,

    UNVRAM_BACKUP_FILE_NOT_EXISTS,
    UNVRAM_BACKUP_FILE_OPEN_FAILED,
    UNVRAM_BACKUP_FILE_READ_FAILED,
    UNVRAM_BACKUP_FILE_STAT_FAILED,
    UNVRAM_COMPLETION_TIMEOUT,

    UNVRAM_FAIL_TO_CLOSE,
    UNVRAM_FAIL_TO_OPEN,
    UNVRAM_FAIL_TO_REQUEST_IO,
    UNVRAM_FAIL_TO_RETRY_IO,
    UNVRAM_PENDING_IO_NOT_FOUND,
    UNVRAM_RESTORING_FAILED,
    UNVRAM_RESTORING_PAGE_DONE,
    UNVRAM_SUBMISSION_FAILED,
    UNVRAM_SUBMISSION_TIMEOUT,

    WRCMP_FAIL_TO_ALLOCATE_MEMORY,
    WRCMP_INVALID_SPLIT_UBIO,
    WRCMP_INVALID_STRIPE,
    WRCMP_IO_ERROR,
    WRCMP_WRITE_WRAPUP_FAILED,
    WRCMP_MAP_UPDATE_FAILED,

    IOCONTROL_REBUILD_FAIL,
    WRHDLR_ALLOC_WRITE_BUFFER_FAILED,
    WRHDLR_FAIL_TO_ALLOCATE_MEMORY,
    WRHDLR_FAIL_BY_SYSTEM_STOP,
    WRHDLR_INVALID_UBIO,

    WRHDLR_NO_FREE_SPACE,
    WRHDLR_DEBUG_READ_OLD,
    WRHDLR_DEBUG_READ_OLD_COMPLETE,

    WRWRAPUP_EVENT_ALLOC_FAILED,
    WRWRAPUP_STRIPE_NOT_FOUND,
    WRWRAPUP_DEBUG_STRIPE,
    BLKMAP_DEBUG_UPDATE_REQUEST,
    BLKMAP_DEBUG_UPDATE,

#if defined NVMe_FLUSH_HANDLING
    FLUSH_CMD_MAPPER_FLUSH_FAILED,
    FLUSH_CMD_ALLOCATOR_FLUSH_FAILED,
    FLUSH_CMD_ONGOING,
#endif

    IOFRONTEND_END,
    IOFRONTEND_COUNT = IOFRONTEND_END - IOFRONTEND_START,

    // --------------IO Path Backend (5300)----------------

    IOBACKEND_START = 5300,

    IOWORKER_OPERATION_NOT_SUPPORTED = IOBACKEND_START,
    IOWORKER_DEVICE_ADDED,
    IOWORKER_UNDERFLOW_HAPPENED,
    IOSMHDLR_BUFFER_NOT_ENOUGH,
    IOSMHDLR_BUFFER_NOT_ALIGNED,
    IOSMHDLR_OPERATION_NOT_SUPPORTED,
    IOSMHDLR_DEBUG_ASYNC_READ,
    IOSMHDLR_COUNT_DIFFERENT,
    IOSMHDLR_DEBUG_ASYNC_WRITE,
    IOSMHDLR_ARRAY_LOCK,
    IOSMHDLR_ARRAY_UNLOCK,

    DEVICE_OPEN_FAILED,
    DEVICE_CLOSE_FAILED,
    DEVICE_SCAN_FAILED,
    DEVICE_SCAN_IGNORED,
    DEVICE_SUBMISSION_FAILED,

    DEVICE_SUBMISSION_QUEUE_FULL,
    DEVICE_SUBMISSION_TIMEOUT,
    DEVICE_COMPLETION_FAILED,
    DEVICE_THREAD_REGISTERED_FAILED,
    DEVICE_THREAD_UNREGISTERED_FAILED,
    DEVICE_OPERATION_NOT_SUPPORTED,
    DEVICE_PENDING_IO_NOT_FOUND,
    DEVICE_UNEXPECTED_PENDING_ERROR_COUNT,
    DEVICE_OVERLAPPED_SET_IOWORKER,
    DEVICE_NULLPTR_IOWORKER,

    QOSMGR_REGISTER_POLLER_FAILED,

    UNVME_SSD_DEBUG_CREATED,
    UNVME_SSD_SCAN_FAILED,
    UNVME_SSD_SCANNED,
    UNVME_SSD_ATTACH_NOTIFICATION_FAILED,
    UNVME_SSD_DETACH_NOTIFICATION_FAILED,
    UNVME_SSD_OPEN_FAILED,
    UNVME_SSD_CLOSE_FAILED,
    UNVME_SSD_UNDERFLOW_HAPPENED,
    UNVME_SUBMISSION_FAILED,
    UNVME_SUBMISSION_QUEUE_FULL,
    UNVME_SUBMISSION_RETRY_EXCEED,
    UNVME_COMPLETION_TIMEOUT,

    UNVME_COMPLETION_FAILED,
    UNVME_OPERATION_NOT_SUPPORTED,
    UNVME_CONTROLLER_FATAL_STATUS,
    UNVME_CONTROLLER_RESET_FAILED,
    UNVME_CONTROLLER_RESET,
    UNVME_SUBMITTING_CMD_ABORT,
    UNVME_ABORT_TIMEOUT,
    UNVME_ABORT_SUBMISSION_FAILED,
    UNVME_ABORT_COMPLETION_FAILED,
    UNVME_ABORT_COMPLETION,
    UNVME_ABORT_DISABLE_AND_RESET,
    UNVME_DO_NOTHING_ON_TIMEOUT,
    UNVME_ABORT_TIMEOUT_FAILED,
    UNVME_ABORT_DISABLE_COUNT_ERR,

    UNVME_NOT_SUPPORTED_DEVICE,

    UNVME_DEBUG_RETRY_IO,
    UNVME_DEBUG_REQUEST_IO,
    UNVME_DEBUG_COMPLETE_IO,

    DEVICE_CONTEXT_NOT_FOUND,

    DEVCTX_ALLOC_TIMEOUT_CHECKER_FAILED,
    IOCTX_ERROR_KEY_NOT_SET,
    IOCTX_IO_KEY_NOT_SET,

    IOQ_ENQUEUE_NULL_UBIO,

    BUFFER_ENTRY_OUT_OF_RANGE,

    NFLSH_ERROR_DETECT,
    NFLSH_EVENT_ALLOCATION_FAILED,
    NFLSH_EVENT_MAP_UPDATE_FAILED,
    NFLSH_STRIPE_NOT_IN_WRITE_BUFFER,
    NFLSH_STRIPE_DEBUG,
    NFLSH_STRIPE_DEBUG_UPDATE,

    FLUSH_WRAPUP_STRIPE_NOT_IN_USER_AREA,
    STRIPEPUTEVT_STRIPE_NOT_IN_NORMAL_POOL,

    IOBACKEND_END,
    IOBACKEND_COUNT = IOBACKEND_END - IOBACKEND_START,

    DEVICE_START = 5500,

    UNVME_DAEMON_START = DEVICE_START,
    UNVME_DAEMON_FINISH,
    UNVME_REGISTER_CTRL,
    UNVME_REGISTER_NS,
    UNVME_PROBE_CALLBACK,
    UNVME_INIT_SCAN_CALLBACK,
    UNVME_ATTACH_CALLBACK,
    UNVME_DETACH_CALLBACK,
    UNVME_SPDK_DETACH,
    UNVME_CLEAN_UP,
    UNVME_INIT_CONTROLLER,
    UNVME_MAX_RETRY_EXCEED,
    UNVME_MAX_TIMEOUT_EXCEED,

    DEVICEMGR_ATTACH,
    DEVICEMGR_CLEAR_DEVICE,
    DEVICEMGR_START_MONITOR,
    DEVICEMGR_STOP_MONITOR,
    DEVICEMGR_CHK_DUPLICATE,
    DEVICEMGR_INIT_SCAN,
    DEVICEMGR_REMOVE_DEV,
    DEVICEMGR_RESCAN,
    DEVICEMGR_GETDEV,
    DEVICEMGR_LISTDEV,
    DEVICEMGR_DETACH,
    DEVICEMGR_SETUPMODEL,

    SYSTEM_START = 9000,
    SYSTEM_SPACE_INFO = SYSTEM_START,
    SYSTEM_UNMOUNT_ERROR,
    SYSTEM_UNMOUNT_PRIORITY_ERROR,
    SYSTEM_ALD_MOUNTED,
    SYSTEM_VERSION,
    SYSTEM_UNMOUNTING,
#if defined QOS_ENABLED_BE
    SYSTEM_EXITING,
#endif

    DEBUG_START = 10000,
    DEBUG_MEMORY_CHECK_DOUBLE_FREE = DEBUG_START,
    DEBUG_MEMORY_CHECK_INVALID_ACCESS,

    // -----------------Exceptions-------------------
    EVENT_ID_MAPPING_WRONG = 0x7FFFFFFE,
    RESERVED = 0x7FFFFFFF,
};

#endif // __IBOF_EVENT_ID_H__
