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

    mkdir -p /tmp/cpuset
    mount -t cpuset none /tmp/cpuset

    log_info "查看cgroup挂载:"
    mount | grep cgroup

    umount /tmp/cpuset
    rmdir /tmp/cpuset

    log_info "After umount, cgroup挂载:"
    mount | grep cgroup
    log_info "All tests completed."
}

main
