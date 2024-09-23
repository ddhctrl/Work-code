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

    if [ ! -d "$CPath/Example" ]; then
        # 如果不存在，则创建该文件夹
        mkdir -p $CPath/Example
    else
        log_info "$CPath/Example already exits."
    fi


    log_info "$CPath/Example/irq.pressure:"
    cat $CPath/Example/irq.pressure

    log_info "$CPath/Example/cgroup.freeze:"
    cat $CPath/Example/cgroup.freeze

    echo "full 10 500000" > $CPath/Example/irq.pressure

    log_info "After write, $CPath/Example/irq.pressure:"
    cat $CPath/Example/irq.pressure

    rmdir $CPath/Example
    log_info "All tests completed."
}

main
