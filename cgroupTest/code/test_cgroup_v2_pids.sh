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

# 创建子cgroup
if [ ! -d "$CPath/Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p $CPath/Example
else
    print_message "INFO" "$CPath/Example already exits."
fi

print_message "INFO" "$CPath/Example/pids.max:"
cat $CPath/Example/pids.max
echo "2" > $CPath/Example/pids.max
print_message "INFO" "write 2 to $CPath/Example/pids.max:"
cat $CPath/Example/pids.max

echo $$ > $CPath/Example/cgroup.procs
print_message "INFO" "$CPath/Example/cgroup.procs: "
cat $CPath/Example/cgroup.procs

# 创建进程并添加
./cgroup_fork_process &
PID1=$!
CPID1=$(pgrep -P $PID1)
echo $CPID1 >> $CPath/Example/cgroup.procs

# 显示信息
print_message "INFO" "$CPath/Example/pids.current: "
cat $CPath/Example/pids.current
print_message "INFO" "$CPath/Example/pids.events: "
cat $CPath/Example/pids.events
print_message "INFO" "$CPath/Example/pids.peak: "
cat $CPath/Example/pids.peak

echo $$ > $CPath/cgroup.procs
rmdir $CPath/Example
