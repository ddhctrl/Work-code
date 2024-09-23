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
cpath_kernfs_id_address=$(cat /proc/kallsyms | grep " cgroup_path_from_kernfs_id$" | awk '{print $1}')
test_ko=ctest_cgroup_path_kfs_id.ko

# 检查地址是否非零
chech_notify_die_addr() {
    if [ "$cpath_kernfs_id_address" != "0000000000000000" ] && [ -n "$cpath_kernfs_id_address" ]; then
        log_info "cpath_kernfs_id_address: $cpath_kernfs_id_address"
    else
        log_error "cpath_kernfs_id_address not found or is zero"
        exit 1
    fi
}

test_notifier_mod() {
    insmod $test_ko addr="0x$cpath_kernfs_id_address"
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

    chech_notify_die_addr
    test_notifier_mod

    log_info "All tests completed."
}

main