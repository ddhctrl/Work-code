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
ignore_signals_address=$(cat /proc/kallsyms | grep " ignore_signals$" | awk '{print $1}')
flush_itimersig_address=$(cat /proc/kallsyms | grep " flush_itimer_signals$" | awk '{print $1}')
unhandled_signal_address=$(cat /proc/kallsyms | grep " unhandled_signal$" | awk '{print $1}')
test_ko=ctest_signal_ignore.ko

# 检查地址是否非零
chech_addr() {
    if [ "$ignore_signals_address" != "0000000000000000" ] && [ -n "$ignore_signals_address" ]; then
        log_info "ignore_signals address: $ignore_signals_address"
    else
        log_error "ignore_signals address not found or is zero"
        exit 1
    fi

    if [ "$flush_itimersig_address" != "0000000000000000" ] && [ -n "$flush_itimersig_address" ]; then
        log_info "flush_itimer_signals address: $flush_itimersig_address"
    else
        log_error "flush_itimer_signals address not found or is zero"
        exit 1
    fi

    if [ "$unhandled_signal_address" != "0000000000000000" ] && [ -n "$unhandled_signal_address" ]; then
        log_info "unhandled_signal address: $unhandled_signal_address"
    else
        log_error "unhandled_signal address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$ignore_signals_address addr2=0x$flush_itimersig_address addr3=0x$unhandled_signal_address
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
