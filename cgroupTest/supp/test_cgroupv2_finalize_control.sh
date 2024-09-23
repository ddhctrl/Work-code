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

CPath=/sys/fs/cgroup
# 获取函数的地址
func_address=$(cat /proc/kallsyms | grep " cgroup_finalize_control$" | awk '{print $1}')
test_ko=cgroupv2_finalize_control.ko

# 检查地址是否非零
chech_addr() {
    if [ "$func_address" != "0000000000000000" ] && [ -n "$func_address" ]; then
        log_info "cgroup_finalize_control: $func_address"
    else
        log_error "cgroup_finalize_control not found or is zero"
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

    if [ ! -d "$CPath/Example" ]; then
        # 如果不存在，则创建该文件夹
        mkdir -p $CPath/Example
    else
        log_info "$CPath/Example already exits."
    fi

    chech_addr
    test_mod

    rmdir $CPath/Example

    log_info "All tests completed."
}

main