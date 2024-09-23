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

#1 读写最大后代数和最大深度
cd $CPath
if [ ! -d "Example3" ]; then
    # 如果不存在，则创建该文件夹
    mkdir Example3
else
    print_message "INFO" "$CPath/Example3 already exits."
fi

print_message "INFO" "Example3 cgroup.max.depth: "
cat $CPath/Example3/cgroup.max.depth

echo 1 > $CPath/Example3/cgroup.max.depth
print_message "INFO" "write 1 to Example3 cgroup.max.depth: "
cat $CPath/Example3/cgroup.max.depth

mkdir $CPath/Example3/Example3_3
if [ $? -eq 0 ]; then
    print_message "INFO" "mkdir $CPath/Example3/Example3_3 success."
    mkdir $CPath/Example3/Example3_3/Example3_3_1
        if [ $? -eq 0 ]; then
            print_message "INFO" "mkdir $CPath/Example3/Example3_3/Example3_3_1 success."
        else
            print_message "ERROR" "mkdir $CPath/Example3/Example3_3/Example3_3_1 failed!"
        fi
else
    print_message "ERROR" "mkdir $CPath/Example3/g1 failed!"
fi

print_message "INFO" "Example3 cgroup.max.descendants: "
cat $CPath/Example3/cgroup.max.descendants

echo 2 > $CPath/Example3/cgroup.max.descendants
print_message "INFO" "write 2 to Example3 cgroup.max.descendants: "
cat $CPath/Example3/cgroup.max.descendants

mkdir $CPath/Example3/Example3_1
if [ $? -eq 0 ]; then
    print_message "INFO" "mkdir $CPath/Example3/Example3_1 success."
else
    print_message "ERROR" "mkdir $CPath/Example3/Example3_1 failed!"
fi

mkdir $CPath/Example3/Example3_2
if [ $? -eq 0 ]; then
    print_message "INFO" "mkdir $CPath/Example3/Example3_2 success."
else
    print_message "ERROR" "mkdir $CPath/Example3/Example3_2 failed!"
fi

rmdir $CPath/Example3/Example3*
rmdir $CPath/Example3