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

# 获取 notify_die 函数的地址
func_address=$(cat /proc/kallsyms | grep " perf_event_read_local$" | awk '{print $1}')
test_ko=ko_perf_event_read_local.ko

# 检查地址是否非零
check_addr() {
    if [ "$func_address" != "0000000000000000" ] && [ -n "$func_address" ]; then
        log_info "perf_event_read_local address: $func_address"
    else
        log_error "perf_event_read_local address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$func_address
    log_info "$test_ko 加载内核模块"
    lsmod
    log_info "$test_ko 卸载模块"
    rmmod $test_ko
}

main() {
    log_info "Start tests..."

    check_addr
    test_mod

    log_info "All tests completed."
}

main