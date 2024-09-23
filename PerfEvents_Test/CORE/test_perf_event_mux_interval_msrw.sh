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

main() {
    log_info "show armv8_pmuv3 perf_event_mux_interval_ms:"
    cat /sys/devices/armv8_pmuv3/perf_event_mux_interval_ms

    log_info "write 5 to armv8_pmuv3 perf_event_mux_interval_ms:"
    echo 5 > /sys/devices/armv8_pmuv3/perf_event_mux_interval_ms
    cat /sys/devices/armv8_pmuv3/perf_event_mux_interval_ms
    echo 4 > /sys/devices/armv8_pmuv3/perf_event_mux_interval_ms
}

main