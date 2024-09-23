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

# 读写cpuset多个属性
print_message "INFO" "$CSPath/cpuset.cpu_exclusive: "
cat $CSPath/cpuset.cpu_exclusive
print_message "INFO" "$CSPath/cpuset.mem_exclusive: "
cat $CSPath/cpuset.mem_exclusive
print_message "INFO" "$CSPath/cpuset.sched_relax_domain_level: "
cat $CSPath/cpuset.sched_relax_domain_level

echo 0 > $CSPath/cpuset.cpu_exclusive
print_message "INFO" "write 0 to $CSPath/cpuset.cpu_exclusive: "
cat $CSPath/cpuset.cpu_exclusive