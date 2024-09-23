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
func1_address=$(cat /proc/kallsyms | grep " force_sig_fault$" | awk '{print $1}')
func2_address=$(cat /proc/kallsyms | grep " send_sig_fault$" | awk '{print $1}')
test_ko=ko_sig_fault_send.ko

# 检查地址是否非零
chech_addr() {
    if [ "$func1_address" != "0000000000000000" ] && [ -n "$func1_address" ]; then
        log_info "force_sig_fault address: $func1_address"
    else
        log_error "force_sig_fault address not found or is zero"
        exit 1
    fi

    if [ "$func2_address" != "0000000000000000" ] && [ -n "$func2_address" ]; then
        log_info "send_sig_fault address: $func2_address"
    else
        log_error "send_sig_fault address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$func1_address addr2=0x$func2_address
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
