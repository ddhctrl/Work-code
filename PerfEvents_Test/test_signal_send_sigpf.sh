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
send_sig_perf_address=$(cat /proc/kallsyms | grep " send_sig_perf$" | awk '{print $1}')
test_ko=ctest_signal_send_sig_perf.ko

# 检查地址是否非零
chech_addr() {
    if [ "$send_sig_perf_address" != "0000000000000000" ] && [ -n "$send_sig_perf_address" ]; then
        log_info "send_sig_perf address: $send_sig_perf_address"
    else
        log_error "send_sig_perf address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$send_sig_perf_address
    log_info "$test_ko 模块加载成功"
    lsmod
    log_info "$test_ko 卸载模块"
    rmmod $test_ko
}

main() {
    log_info "Start tests..."

    chech_addr
    test_mod

    log_info "All tests completed."
}

main
