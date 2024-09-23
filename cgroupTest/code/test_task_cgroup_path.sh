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

test_ko=ctest_task_cgroup_path.ko

test_notifier_mod() {
    insmod $test_ko pid=$$
    if [ $? -eq 0 ]; then
        log_info "$test_ko 模块加载成功"
    else
        log_error "$test_ko 模块加载失败"
        exit 1
    fi
    lsmod | grep cgroup
    log_info "$test_ko 卸载模块"
    rmmod $test_ko
}

main() {
    log_info "Start tests..."

    test_notifier_mod

    log_info "All tests completed."
}

main