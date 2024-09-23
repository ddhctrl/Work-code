#!/bin/bash
CSPath=/sys/fs/cgroup/cpuset

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

#1 创建cpuset子cgroup
echo 1 > $CSPath/cgroup.clone_children
if [ ! -d "$CSPath/Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p $CSPath/Example
else
    print_message "INFO" "$CSPath/Example already exits."
fi

#2 设置绑定cpu1
echo 1 > $CSPath/Example/cpuset.cpus
print_message "INFO" "$CSPath/Example/cpuset.cpus: "
cat $CSPath/Example/cpuset.cpus
print_message "INFO" "$CSPath/Example/cpuset.mems: "
cat $CSPath/Example/cpuset.mems

#3 添加进程
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)
echo $CPID >> $CSPath/Example/cgroup.procs
print_message "INFO" "$CSPath/Example/cgroup.procs: "
cat $CSPath/Example/cgroup.procs

#4 热插拔cpu1
print_message "INFO" "CPU: "
lscpu | grep CPU
print_message "INFO" "########CPU Hotplug########"
echo 0 > /sys/devices/system/cpu/cpu1/online
lscpu | grep CPU

print_message "INFO" "$CSPath/Example/cpuset.cpus: "
cat $CSPath/Example/cpuset.cpus
print_message "INFO" "$CSPath/Example/cgroup.procs: "
cat $CSPath/Example/cgroup.procs

echo 1 > /sys/devices/system/cpu/cpu1/online
kill -9 $CPID
rmdir $CSPath/Example