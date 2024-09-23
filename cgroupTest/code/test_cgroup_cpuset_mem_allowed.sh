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

# 创建进程
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)

if [ -z "$PID" ]; then
     print_message "ERROR" "PPID get failed!"
     exit 1
fi

if [ -z "$CPID" ]; then
     print_message "ERROR""CPID get failed!"
     exit 1
fi

# 加载内核模块
MODULE=ctest_cgroup_cpuset_vma_alloc.ko
if [ ! -f "$MODULE" ]; then
    print_message "ERROR" "$MODULE does not exits!"
    exit 1
fi

insmod $MODULE pid=$CPID
lsmod
rmmod $MODULE
dmesg | tail

kill -9 $CPID
