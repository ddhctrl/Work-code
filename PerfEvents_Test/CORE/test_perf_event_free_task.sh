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
    # 创建子cgroup
    if [ ! -d "$CPath/Example" ]; then
        # 如果不存在，则创建该文件夹
        mkdir -p $CPath/Example
    else
        log_info "$CPath/Example already exits."
    fi

    echo 2 > $CPath/Example/pids.max
    log_info "write 2 to CPath/Example/pids.max:"
    cat $CPath/Example/pids.max

    echo $$ > $CPath/Example/cgroup.procs
    log_info "$CPath/Example/cgroup.procs"
    cat $CPath/Example/cgroup.procs

    ./ctest_perf_event_syn_stat

    echo $$ > $CPath/cgroup.procs
    rmdir $CPath/Example

    log_info "All tests completed."
}

main
