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

test_perf_commond() {
    if ! command -v perf &> /dev/null; then
    log_error "perf 命令未找到。请安装 perf 工具。"
    exit 1
    fi

    log_info "perf 命令可用"
}

test_perf_exceution() {
    perf top &
    PERF_PID=$!
    sleep 10
    kill -SIGINT $PERF_PID

    rm -rf perf.data
    log_info "perf is recording cycles, wait 10s..."
    perf record -e cycles -o perf.data -- sleep 10

    if [ ! -f perf.data ]; then
    log_error "No perf.data create!"
    exit 1
    fi
}

main() {
    log_info "Start tests..."

    test_perf_commond
    test_perf_exceution

    log_info "All tests completed."
}

main