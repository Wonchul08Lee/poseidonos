#!/bin/bash
cd $(dirname $(realpath $0))
IBOFOS=../../bin/ibofos
DUMP_PATH=/etc/ibofos/core
IBOFOS_LOG_PATH=/var/log/ibofos/
IBOFOS_COPY=./ibofos

PROC_PID=`ps -ef | egrep "*/ibofos$" | awk '{print $2}'`
PID="$PROC_PID"
echo $PID
DATE=`date "+%Y%m%d_%H%M%S"`

CORE_PREFIX=ibofos.core.$DATE
CORE_FILE=ibofos.core.$DATE.$PID
CORE_CRASHED=ibofos.core
LIBRARY_FILE=library.tar.gz

BINARY_INFO=binary.info
LOG_SUFFIX="log"
IN_MEMORY_LOG_FILE=""
CALLSTACK_INFO=""
PENDINGIO_INFO=""
TRIGGER_LOG=trigger.log

CRASH=-1
mkdir -p $DUMP_PATH

log_normal(){
    echo -e "\033[32m"$1"\033[0m"
}

log_error(){
    echo -e "\033[31m"$1"\033[0m"
}

get_first_core_information(){
    rm -rf ibofos.inmemory.log call_stack.info pending_io.info
    #If we cannot get information within 5 minutes, we just skip first core information.
    timeout 300s ./get_first_info_from_dump.sh 1>$TRIGGER_LOG 2>$TRIGGER_LOG
    if [ ${?} -ne 0 ];then
        log_error "Timeout happend during get_first_info_from_dump.sh"
    fi
    if [ -f "ibofos.inmemory.log" ];then
        IN_MEMORY_LOG_FILE="ibofos.inmemory.log"
        log_normal "dump log (in memory log) successfully retrived"
    else
        echo "dump log cannot be retrieved in this phase (please use dumplog option in load_dump.sh)"
    fi
    if [ -f "call_stack.info" ];then
        CALLSTACK_INFO="call_stack.info"
        log_normal "call stack info successfully retrived"
    else
        echo "call_stack info cannot be retrived"
    fi
    if [ -f "pending_io.info" ];then
        PENDINGIO_INFO="pending_io.info"
        log_normal "pending io information successfully retrived"
    else
        echo "pending io info cannot be retrived"
    fi

}

LOG_ONLY=0

if [ "$#" -gt 0 ]; then
    FLAG_CRASH=$1
    echo $FLAG_CRASH

    if [ $FLAG_CRASH == "triggercrash" ];then
        rm -rf $DUMP_PATH"/"$CORE_CRASHED
        CRASH=1
    fi

    if [ $FLAG_CRASH == "crashed" ];then
        CRASH=2
        PID="crashed"
        CORE_FILE=ibofos.core.$DATE.$PID
    fi

    if [ $FLAG_CRASH == "gcore" ];then
        CRASH=0
    fi

    if [ $FLAG_CRASH == "logonly" ];then
        CRASH=0
        PID="logonly"
        LOG_ONLY=1
        CORE_FILE=ibofos.core.$DATE.$PID
    fi
fi

LOG_DIR="logs"
mkdir -p $LOG_DIR

if [ $CRASH -eq -1 ];then
    log_error "please input option [triggercrash, crashed, gcore, logonly]"
    exit
fi

# if flag indicates not crashed, try to gather the ibofos in memory log with best effort

if [ ! -z $PROC_PID ]; then
    get_first_core_information
fi

# if test want to indicate log as unique id, please use first param of this script
if [ "$#" -gt 2 ]; then
    UNIQUE_ID=$2
    CORE_PREFIX=core.$UNIQUE_ID.$DATE
fi

if [ -f $IBOFOS ];then
    echo "ibofos binary check"
    cp --preserve=timestamps $IBOFOS $IBOFOS_COPY
else
    log_error "set IBOFOS path correctly"
    exit
fi



if [ $CRASH -ne 0 ];then
    if [ $CRASH -eq 1 ];then
        echo "####### Triger Crash dump #######"
        #11 is segementation fault, and it leaves crash dump.
        python3 -c 'import ps_lib; ps_lib.kill_wait("kill")'
        log_normal "#### Process killed ######"
    else
        python3 -c "import ps_lib; ps_lib.kill_wait()"
        echo "####### Get Crash dump #######"
    fi

    PREV_SIZE=-1
    RETRY_COUNT=6
    DUMP_CREATED=0
    python3 -c "import ps_lib; ps_lib.find_ibofos_coredump_and_renaming()"
    echo $CORE_CRASHED

    for i in `seq 1 $RETRY_COUNT`
    do
        if [ -f $DUMP_PATH"/"$CORE_CRASHED ];then
          NSIZE=`ls -l $DUMP_PATH"/"$CORE_CRASHED | awk '{print $5}'`
        fi
        if [ -z $NSIZE ]; then
            continue
        fi
        if [ $PREV_SIZE == $NSIZE ]; then
            DUMP_CREATED=1
            break
        fi
        PREV_SIZE=$NSIZE
        sleep 5
    done

    if [ $DUMP_CREATED -eq 1 ];then
        cp --preserve=timestamps $DUMP_PATH"/"$CORE_CRASHED ./$CORE_FILE
    else
        log_error "#### Dump File is not created, please check DUMP_PATH or IBOFOS running######"
        exit
    fi

else
    if [ $LOG_ONLY -eq 0 ];then
        echo "####### Trriger Gcore dump #######"
        gcore -o $CORE_PREFIX $PID
    fi
fi


if [ -z $PID ];then
    if [ $CRASH -le 1 ];then
        log_error "Please Check if Ibofos is running"
        exit
    fi
fi

if [ -z $PROC_PID ]; then
    get_first_core_information
fi

if [ $? -ne 0 ];then
    log_error "Gcore dump failed"
    exit
fi

echo "####### Gcore dump is finished #######"
echo "####### Compression start ######"

./collect_library.py library.tar.gz

if [ $? -ne 0 ];then
    log_error "Gcore dump failed"
    exit
fi

#gzip automatically input file.

cp $IBOFOS_LOG_PATH/* $LOG_DIR -rf
cp /var/log/syslog /var/log/syslog.1 $LOG_DIR -rf
dmesg > $LOG_DIR"/dmesg.log"
./collect_binary_info.sh > $BINARY_INFO

if [ $LOG_ONLY -eq 0 ];then
    tar czf - $CALLSTACK_INFO $PENDINGIO_INFO $IN_MEMORY_LOG_FILE $LOG_DIR $IBOFOS_COPY $BINARY_INFO library.tar.gz | split -b 70m - $CORE_FILE.$LOG_SUFFIX.tar.gz 
    tar czf - $CORE_FILE $IBOFOS_COPY $BINARY_INFO library.tar.gz | split -b 70m - $CORE_FILE.tar.gz 
else	
    tar czf - $CALLSTACK_INFO $PENDINGIO_INFO $IN_MEMORY_LOG_FILE $LOG_DIR $IBOFOS_COPY $BINARY_INFO library.tar.gz | split -b 70m - $CORE_FILE.tar.gz 
fi	

if [ $? -ne 0 ];then
    log_error "Compression from core file is failed"
    exit
fi

rm $CORE_FILE -rf
rm $LOG_DIR -rf

echo "####### Tar split compression is finished #######"
log_normal "#######"$CORE_FILE".tar.gzxx is successfully created, Please Attach this file to IMS #######"
if [ $LOG_ONLY -eq 0 ];then
    log_normal "###### new file "$CORE_FILE"."$LOG_SUFFIX".tar.gzxx is successfully created, Please Also Attach this file to IMS #######"
fi
