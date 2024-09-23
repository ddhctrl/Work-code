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

# 获取函数的地址
func_address1=$(cat /proc/kallsyms | grep " event_function_local.constprop.0$" | awk '{print $1}')
func_address2=$(cat /proc/kallsyms | grep " __perf_event_disable$" | awk '{print $1}')
test_ko=ko_perf_event_function_local.ko

# 检查地址是否非零
check_addr() {
    if [ "$func_address1" != "0000000000000000" ] && [ -n "$func_address1" ]; then
        log_info "event_function_local address: $func_address1"
    else
        log_error "event_function_local address not found or is zero"
        exit 1
    fi

    if [ "$func_address2" != "0000000000000000" ] && [ -n "$func_address2" ]; then
        log_info "__perf_event_disable address: $func_address2"
    else
        log_error "__perf_event_disable address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$func_address1 addr2=0x$func_address2
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