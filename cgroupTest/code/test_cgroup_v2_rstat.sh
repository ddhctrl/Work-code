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
echo "+cpuset +cpu" >> $CPath/cgroup.subtree_control
print_message "INFO" "$CPath/Example/cgroup.controllers"

# 创建进程并添加, 显示信息
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)
echo $CPID >> $CPath/Example/cgroup.procs
print_message "INFO" "$CPath/Example/cgroup.procs: "
cat $CPath/Example/cgroup.procs
sleep 3
print_message "INFO" "$CPath/Example/cpu.stat: "
cat $CPath/Example/cpu.stat

kill -9 $CPID
rmdir $CPath/Example
