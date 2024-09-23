#!/bin/bash
CPath=/sys/fs/cgroup
FPath=/sys/kernel/example/fd
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

insmod ctest_cgroup_get_from_file.ko
lsmod | grep cgroup
./ctest_cgroup_file_open
./ctest_cgroup_filepath
rmmod ctest_cgroup_get_from_file.ko
dmesg | tail