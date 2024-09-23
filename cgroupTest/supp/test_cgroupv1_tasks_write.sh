#!/bin/bash
CSPath=/sys/fs/cgroup/cpuset
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

    if [ ! -d "$CSPath/Example" ]; then
        # 如果不存在，则创建该文件夹
        mkdir -p $CSPath/Example
    else
        log_info "$CSPath/Example already exits."
    fi

    echo 0 > $CSPath/Example/cpuset.cpus
    echo 0 > $CSPath/Example/cpuset.mems

    ./cgroup_fork_process &
    PID=$!
    CPID=$(pgrep -P $PID)
    echo $CPID >>$CSPath/Example/tasks
    log_info "$CSPath/Example/tasks: "
    cat $CSPath/Example/tasks

    kill -9 $CPID
    rmdir $CSPath/Example
    log_info "All tests completed."
}

main
