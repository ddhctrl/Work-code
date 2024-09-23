#!/bin/bash
print_message() {
    local message_type=$1
    shift
    local message="$@"
    case "$message_type" in
        "INFO")
            echo -e "\033[1;32m[INFO]\033[0m $message"
            ;;
        "WARNING")
            echo -e "\033[1;33m[WARNING]\033[0m $message"
            ;;
        "ERROR")
            echo -e "\033[1;31m[ERROR]\033[0m $message"
            ;;
        *)
            echo "[UNKNOWN] $message"
            ;;
    esac
}

ko=ctest_cgroup_v1_attach_task.ko
if [ ! -f $ko ]; then
   print_message "ERROR" "$ko does not exits!"
   exit 1
fi

#1 创建2个进程
./cgroup_fork_process &
PID1=$!
CPID1=$(pgrep -P $PID1)

if [ -z "$PID1" ]; then
     print_message "ERROR" "PPID1 get failed!"
     exit 1
fi

if [ -z "$CPID1" ]; then
     print_message "ERROR""CPID1 get failed!"
     exit 1
fi

./cgroup_fork_process &
PID2=$!
CPID2=$(pgrep -P $PID2)

if [ -z "$PID2" ]; then
     print_message "ERROR" "PPID2 get failed!"
     exit 1
fi

if [ -z "$CPID2" ]; then
     print_message "ERROR""CPID2 get failed!"
     exit 1
fi

#2 加载内核模块
insmod ctest_cgroup_v1_attach_task.ko pid_from=$CPID1 pid_to=$CPID2
print_message "INFO" "lsmod: "
lsmod
rmmod ctest_cgroup_v1_attach_task.ko
kill -9 $CPID1 $CPID2
dmesg | tail