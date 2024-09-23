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

    if [ ! -d $CPath/freezer ]; then
        log_error "$CPath/freezer does not exits!"
        exit 1
    fi

    if [ ! -d "$CPath/freezer/Example" ]; then
        # 如果不存在，则创建该文件夹
        mkdir -p $CPath/freezer/Example
    else
        log_info "$CPath/freezer/Example already exits."
    fi

    log_info "$CPath/freezer/Example/freezer.parent_freezing:"
    cat $CPath/freezer/Example/freezer.parent_freezing

    log_info "$CPath/freezer/Example/freezer.self_freezing:"
    cat $CPath/freezer/Example/freezer.self_freezing

    rmdir $CPath/freezer/Example
    log_info "All tests completed."
}

main
