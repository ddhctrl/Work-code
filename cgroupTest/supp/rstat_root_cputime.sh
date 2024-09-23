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

print_message "INFO" "$CPath/cpu.stat: "
cat $CPath/cpu.stat
