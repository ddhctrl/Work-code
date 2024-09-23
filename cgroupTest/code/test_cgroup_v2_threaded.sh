#!/bin/bash
CPath=/sys/fs/cgroup
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
#1 cgroup线程化
cd $CPath
if [ ! -d "Example2" ]; then
    # 如果不存在，则创建该文件夹
    mkdir Example2
else
    print_message "INFO" "$CPath/Example2 already exits."
fi

print_message "INFO" "Example2 cgroup.type :"
cat $CPath/Example2/cgroup.type

echo "threaded" > $CPath/Example2/cgroup.type
print_message "INFO" "Threaded Example2 cgroup.type :"
cat $CPath/Example2/cgroup.type

cd -
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

echo $CPID >> $CPath/Example2/cgroup.procs
print_message "INFO" "Example2 cgroup.procs :"

kill -9 $CPID

rmdir $CPath/Example2
if [ $? -eq 0 ]; then
    print_message "INFO" "rmdir $CPath/Example2 success."
else
    print_message "ERROR" "rmdir $CPath/Example2 failed!"
fi