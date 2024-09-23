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
func_address=$(cat /proc/kallsyms | grep " rb_alloc_aux$" | awk '/rb_alloc_aux/ {print $1}')
test_ko=rb_alloc_aux.ko

# 检查地址是否非零
chech_addr() {
    if [ "$func_address" != "0000000000000000" ] && [ -n "$func_address" ]; then
        log_info "rb_alloc_aux address: $func_address"
    else
        log_error "rb_alloc_aux address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$func_address
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
