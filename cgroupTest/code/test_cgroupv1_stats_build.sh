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
cgroupstats_build_address=$(cat /proc/kallsyms | grep " cgroupstats_build$" | awk '{print $1}')
test_ko=ctest_cgroupv1_stats_build.ko

# 检查地址是否非零
chech_addr() {
    if [ "$cgroupstats_build_address" != "0000000000000000" ] && [ -n "$cgroupstats_build_address" ]; then
        log_info "cgroupstats_build_address: $cgroupstats_build_address"
    else
        log_error "cgroupstats_build_address not found or is zero"
        exit 1
    fi
}

test_notifier_mod() {
    insmod $test_ko addr="0x$cgroupstats_build_address"
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