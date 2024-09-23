#!/bin/bash
log_info() {
    echo -e "$(date +'%Y-%m-%d %H:%M:%S') \033[1;32m[INFO]\033[0m $1"
}
log_warn() {
    echo -e "$(date +'%Y-%m-%d %H:%M:%S') \033[1;33m[WARNING]\033[0m $1"
}
log_error() {
    echo -e "$(date +'%Y-%m-%d %H:%M:%S') \033[1;31m[ERROR]\033[0m $1"
}

test_exe=ctest_perf_event_min_heap
cgroup_perf_path=/sys/fs/cgroup/perf_event

do_pre_work() {
    if [ ! -d "$cgroup_perf_path" ]; then
        log_error "$cgroup_perf_path not exist!"
        exit 1
    fi

    mkdir $cgroup_perf_path/Example
    echo $$ >> $cgroup_perf_path/Example/cgroup.procs
    log_info "将当前shell加入到新建cgroup中, $cgroup_perf_path/Example/cgroup.procs:"
    cat $cgroup_perf_path/Example/cgroup.procs
}

do_exit_work() {
    echo $$ >> $cgroup_perf_path/cgroup.procs
    rmdir $cgroup_perf_path/Example
}

main() {
    log_info "Start tests..."
    
    do_pre_work
    ./$test_exe
    do_exit_work

    log_info "All tests completed."
}

main