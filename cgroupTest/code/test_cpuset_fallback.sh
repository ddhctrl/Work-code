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
cpuset_allowed_fallbcak_address=$(cat /proc/kallsyms | grep " cpuset_cpus_allowed_fallback$" | awk '{print $1}')
test_ko=ctest_cpuset_fallback.ko

# 检查地址是否非零
chech_addr() {
    if [ "$cpuset_allowed_fallbcak_address" != "0000000000000000" ] && [ -n "$cpuset_allowed_fallbcak_address" ]; then
        log_info "cpuset_cpus_allowed_fallback: $cpuset_allowed_fallbcak_address"
    else
        log_error "cpuset_cpus_allowed_fallback not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr="0x$cpuset_allowed_fallbcak_address"
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