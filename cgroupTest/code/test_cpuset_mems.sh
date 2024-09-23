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
cpuset_print_memscua_address=$(cat /proc/kallsyms | grep " cpuset_print_current_mems_allowed$" | awk '{print $1}')
cpuset_mem_pressure_address=$(cat /proc/kallsyms | grep " __cpuset_memory_pressure_bump$" | awk '{print $1}')
test_ko=ctest_cpuset_mems.ko

# 检查地址是否非零
chech_addr1() {
    if [ "$cpuset_print_memscua_address" != "0000000000000000" ] && [ -n "$cpuset_print_memscua_address" ]; then
        log_info "cpuset_print_current_mems_allowed: $cpuset_print_memscua_address"
    else
        log_error "cpuset_print_current_mems_allowed not found or is zero"
        exit 1
    fi
}

chech_addr2() {
    if [ "$cpuset_mem_pressure_address" != "0000000000000000" ] && [ -n "$cpuset_mem_pressure_address" ]; then
        log_info "__cpuset_memory_pressure_bump: $cpuset_mem_pressure_address"
    else
        log_error "__cpuset_memory_pressure_bump not found or is zero"
        exit 1
    fi
}

test_notifier_mod() {
    insmod $test_ko addr1="0x$cpuset_print_memscua_address" addr2="0x$cpuset_mem_pressure_address"
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

    chech_addr1
    chech_addr2
    test_notifier_mod

    log_info "All tests completed."
}

main