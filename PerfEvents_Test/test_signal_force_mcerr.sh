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
force_sig_mceerr_address=$(cat /proc/kallsyms | grep " force_sig_mceerr$" | awk '{print $1}')
force_sig_bnderr_address=$(cat /proc/kallsyms | grep " force_sig_bnderr$" | awk '{print $1}')
force_sig_pkuerr_address=$(cat /proc/kallsyms | grep " force_sig_pkuerr$" | awk '{print $1}')
send_sig_perf_address=$(cat /proc/kallsyms | grep " send_sig_perf$" | awk '{print $1}')
test_ko=ctest_signal_force_sig_mceerr.ko

# 检查地址是否非零
chech_addr() {
    if [ "$force_sig_mceerr_address" != "0000000000000000" ] && [ -n "$force_sig_mceerr_address" ]; then
        log_info "force_sig_mceerr address: $force_sig_mceerr_address"
    else
        log_error "force_sig_mceerr address not found or is zero"
        exit 1
    fi

    if [ "$force_sig_bnderr_address" != "0000000000000000" ] && [ -n "$force_sig_bnderr_address" ]; then
        log_info "force_sig_bnderr address: $force_sig_bnderr_address"
    else
        log_error "force_sig_bnderr address not found or is zero"
        exit 1
    fi

    if [ "$force_sig_pkuerr_address" != "0000000000000000" ] && [ -n "$force_sig_pkuerr_address" ]; then
        log_info "force_sig_pkuerr address: $force_sig_pkuerr_address"
    else
        log_error "force_sig_pkuerr address not found or is zero"
        exit 1
    fi

    if [ "$send_sig_perf_address" != "0000000000000000" ] && [ -n "$send_sig_perf_address" ]; then
        log_info "send_sig_perf address: $send_sig_perf_address"
    else
        log_error "send_sig_perf address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$force_sig_mceerr_address addr2=0x$force_sig_bnderr_address addr3=0x$force_sig_pkuerr_address \ 
    addr4=0x$send_sig_perf_address
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
