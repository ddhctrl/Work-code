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
cal_sigpending_address=$(cat /proc/kallsyms | grep " calculate_sigpending$" | awk '{print $1}')
recalc_sigpending_address=$(cat /proc/kallsyms | grep " recalc_sigpending_and_wake$" | awk '{print $1}')
test_ko=ctest_signal_sig_pend.ko

# 检查地址是否非零
chech_addr() {
    if [ "$cal_sigpending_address" != "0000000000000000" ] && [ -n "$cal_sigpending_address" ]; then
        log_info "calculate_sigpending address: $cal_sigpending_address"
    else
        log_error "calculate_sigpending address not found or is zero"
        exit 1
    fi

    if [ "$recalc_sigpending_address" != "0000000000000000" ] && [ -n "$recalc_sigpending_address" ]; then
        log_info "recalc_sigpending_and_wake address: $recalc_sigpending_address"
    else
        log_error "recalc_sigpending_and_wake address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$cal_sigpending_address addr2=0x$recalc_sigpending_address
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
