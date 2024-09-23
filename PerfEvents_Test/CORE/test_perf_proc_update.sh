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

write_perf_event_max_sample_rate() {
    log_info "限制性能事件采样的最大速率: "
    cat /proc/sys/kernel/perf_event_max_sample_rate

    log_info "设置限制性能事件采样的最大速率为2000: "
    echo 2000 > /proc/sys/kernel/perf_event_max_sample_rate
    cat /proc/sys/kernel/perf_event_max_sample_rate

    echo 9750 > /proc/sys/kernel/perf_event_max_sample_rate
}

main() {
    log_info "Start tests..."
    write_perf_event_max_sample_rate
    log_info "All tests completed."
}

main