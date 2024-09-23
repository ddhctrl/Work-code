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
target_dir=/proc/sys/kernel/perf_cpu_time_max_percent
write_perf_event_max_sample_rate() {
    log_info "性能事件采样的cpu时间最大百分比: "
    cat $target_dir

    log_info "设置性能事件采样的cpu时间最大百分比为50: "
    echo 50 > $target_dir
    cat $target_dir

    echo 25 > $target_dir
}

main() {
    log_info "Start tests..."
    write_perf_event_max_sample_rate
    log_info "All tests completed."
}

main