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

    if [ ! -d $CPath/debug ]; then
        log_error "$CPath/debug does not exits!"
        exit 1
    fi

    if [ ! -d "$CPath/debug/Example" ]; then
        # 如果不存在，则创建该文件夹
        mkdir -p $CPath/debug/Example
    else
        log_info "$CPath/debug/Example already exits."
    fi


    log_info "$CPath/debug/debug.cgroup_css_links:"
    cat $CPath/debug/debug.cgroup_css_links

    log_info "$CPath/debug/debug.current_css_set_cg_links:"
    cat $CPath/debug/debug.current_css_set_cg_links

    log_info "$CPath/debug/debug.current_css_set_refcount:"
    cat $CPath/debug/debug.current_css_set_refcount

    log_info "$CPath/debug/debug.releasable:"
    cat $CPath/debug/debug.releasable

    rmdir $CPath/debug/Example
    log_info "All tests completed."
}

main
