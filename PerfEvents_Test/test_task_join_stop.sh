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
task_stop_address=$(cat /proc/kallsyms | grep " task_join_group_stop$" | awk '{print $1}')
test_ko=ctest_task_join_stop.ko

# 检查地址是否非零
chech_addr() {
    if [ "$task_stop_address" != "0000000000000000" ] && [ -n "$task_stop_address" ]; then
        log_info "task_join_group_stop address: $task_stop_address"
    else
        log_error "task_join_group_stop address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$task_stop_address
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
