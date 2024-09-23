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

#1 读写信息
cd $CPath
if [ ! -d "Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir Example
else
    print_message "INFO" "$CPath/Example already exits."
fi

print_message "INFO" "Example cgroup.events: "
cat $CPath/Example/cgroup.events

print_message "INFO" "Example cgroup.stat: "
cat $CPath/Example/cgroup.stat

#2 设置io.pressure阈值和周期
echo "some 150000 1000000" > $CPath/Example/io.pressure
print_message "INFO" "Example io.pressure: "
cat $CPath/Example/io.pressure

#3 设置memory.pressure阈值和周期
echo "some 150000 1000000" > $CPath/Example/memory.pressure
print_message "INFO" "Example memory.pressure: "
cat $CPath/Example/memory.pressure

#4 设置cpu.pressure阈值和周期
echo "some 150000 1000000" > $CPath/Example/cpu.pressure
print_message "INFO" "Example cpu.pressure: "
cat $CPath/Example/cpu.pressure

#5 开关cgroup PSI计数
echo 0 > $CPath/Example/cgroup.pressure
print_message "INFO" "Example cgroup.pressure: "
cat $CPath/Example/cgroup.pressure
echo 1 > $CPath/Example/cgroup.pressure
print_message "INFO" "Example cgroup.pressure: "
cat $CPath/Example/cgroup.pressure

#6 使用poll查询
cd -
./ctest_cgroup_poll &

sleep 3

rmdir $CPath/Example