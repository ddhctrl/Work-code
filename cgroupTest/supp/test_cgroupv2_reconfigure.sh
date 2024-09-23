#!/bin/bash
CPath=/sys/fs/cgroup
log_info() {
    echo -e "$(date +'%Y-%m-%d %H:%M:%S') \033[1;32m[INFO]\033[0m $1"
}
log_warn() {
    echo -e "$(date +'%Y-%m-%d %H:%M:%S') \033[1;33m[WARNING]\033[0m $1"
}
log_error() {
    echo -e "$(date +'%Y-%m-%d %H:%M:%S') \033[1;31m[ERROR]\033[0m $1"
}

main() {
    log_info "Start tests..."

    mkdir -p /tmp/cgroup
    mount -t cgroup2 none /tmp/cgroup

    log_info "查看cgroup挂载:"
    mount | grep cgroup

    mount -o remount -t cgroup2 none /tmp/cgroup

    umount /tmp/cgroup
    rmdir /tmp/cgroup

    log_info "After umount, cgroup挂载:"
    mount | grep cgroup
    log_info "All tests completed."
}

main
