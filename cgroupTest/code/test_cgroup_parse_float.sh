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
cparse_float_address=$(cat /proc/kallsyms | grep " cgroup_parse_float$" | awk '{print $1}')
test_ko=ctest_cgroup_parse_float.ko

# 检查地址是否非零
chech_addr() {
    if [ "$cparse_float_address" != "0000000000000000" ] && [ -n "$cparse_float_address" ]; then
        log_info "cparse_float_address: $cparse_float_address"
    else
        log_error "cparse_float_address not found or is zero"
        exit 1
    fi
}

test_notifier_mod() {
    insmod $test_ko addr="0x$cparse_float_address"
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
    test_notifier_mod

    log_info "All tests completed."
}

main