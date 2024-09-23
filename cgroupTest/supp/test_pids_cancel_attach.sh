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
func_address=$(cat /proc/kallsyms | grep " pids_cancel_attach$" | awk '{print $1}')
test_ko=pids_cancel_attach.ko

# 检查地址是否非零
chech_addr() {
    if [ "$func_address" != "0000000000000000" ] && [ -n "$func_address" ]; then
        log_info "pids_cancel_attach: $func_address"
    else
        log_error "pids_cancel_attach not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr="0x$func_address"
    if [ $? -eq 0 ]; then
        log_info "$test_ko 模块加载成功"
    else
        log_error "$test_ko 模块加载失败"
        exit 1
    fi
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