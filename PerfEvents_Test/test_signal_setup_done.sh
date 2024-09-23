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
signal_setupdn_address=$(cat /proc/kallsyms | grep " signal_setup_done$" | awk '{print $1}')
force_exit_sig_address=$(cat /proc/kallsyms | grep " force_exit_sig$" | awk '{print $1}')
test_ko=ctest_signal_setup_done.ko

# 检查地址是否非零
chech_addr() {
    if [ "$signal_setupdn_address" != "0000000000000000" ] && [ -n "$signal_setupdn_address" ]; then
        log_info "signal_setup_done address: $signal_setupdn_address"
    else
        log_error "signal_setup_done address not found or is zero"
        exit 1
    fi

    if [ "$force_exit_sig_address" != "0000000000000000" ] && [ -n "$force_exit_sig_address" ]; then
        log_info "force_exit_sig address: $force_exit_sig_address"
    else
        log_error "force_exit_sig address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$signal_setupdn_address addr2=0x$force_exit_sig_address
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
