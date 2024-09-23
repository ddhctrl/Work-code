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
notify_die_address=$(cat /proc/kallsyms | grep " notify_die$" | awk '{print $1}')
test_ko=notifier.ko

# 检查地址是否非零
chech_notify_die_addr() {
    if [ "$notify_die_address" != "0000000000000000" ] && [ -n "$notify_die_address" ]; then
        log_info "notify_die address: $notify_die_address"
    else
        log_error "notify_die address not found or is zero"
        exit 1
    fi
}

test_notifier_mod() {
    insmod $test_ko addr=$notify_die_address
    log_info "$test_ko 模块加载成功"
    lsmod
    log_info "$test_ko 卸载模块"
    rmmod $test_ko
}

main() {
    log_info "Start tests..."

    chech_notify_die_addr
    test_notifier_mod

    log_info "All tests completed."
}

main