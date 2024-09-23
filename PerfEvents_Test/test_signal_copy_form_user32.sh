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
func_address=$(cat /proc/kallsyms | grep " copy_siginfo_from_user32$" | awk '/copy_siginfo_from_user32/ {print $1}')
test_ko=signal_copy_from_user32.ko

# 检查地址是否非零
chech_addr() {
    if [ "$func_address" != "0000000000000000" ] && [ -n "$func_address" ]; then
        log_info "copy_siginfo_from_user32 address: $func_address"
    else
        log_error "copy_siginfo_from_user32 address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$func_address
    if lsmod | grep -q "${test_ko}"; then
        log_info "$test_ko 模块加载成功"
        log_info "$test_ko 卸载模块"
        rmmod $test_ko
    else
        log_error "Failed to load module"
        exit 1
    fi
}

main() {
    log_info "Start tests..."

    chech_addr
    test_mod

    log_info "All tests completed."
}

main
