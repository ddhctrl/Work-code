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

if [ ! -d $CPath/debug ]; then
    print_message "ERROR" "$CPath/debug does not exits!"
    exit 1
fi

#显示信息
print_message "INFO" "$CPath/debug/debug.taskcount: "
cat $CPath/debug/debug.taskcount
print_message "INFO" "$CPath/debug/debug.current_css_set: "
cat $CPath/debug/debug.current_css_set
print_message "INFO" "$CPath/debug/debug.cgroup_subsys_states: "
cat $CPath/debug/debug.cgroup_subsys_states
print_message "INFO" "$CPath/debug/debug.cgroup_masks: "
cat $CPath/debug/debug.cgroup_masks
print_message "INFO" "/proc/self/cpuset: "
cat /proc/self/cpuset