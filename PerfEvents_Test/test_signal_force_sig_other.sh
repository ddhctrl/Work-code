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
force_sig_seccomp_address=$(cat /proc/kallsyms | grep " force_sig_seccomp$" | awk '{print $1}')
fcesig_ptrace_errno_trap_address=$(cat /proc/kallsyms | grep " force_sig_ptrace_errno_trap$" | awk '{print $1}')
force_sig_fault_trapno_address=$(cat /proc/kallsyms | grep " force_sig_fault_trapno$" | awk '{print $1}')
send_sig_fault_trapno_address=$(cat /proc/kallsyms | grep " send_sig_fault_trapno$" | awk '{print $1}')
test_ko=ctest_signal_force_sig_other.ko

# 检查地址是否非零
chech_addr() {
    if [ "$force_sig_seccomp_address" != "0000000000000000" ] && [ -n "$force_sig_seccomp_address" ]; then
        log_info "force_sig_seccomp address: $force_sig_seccomp_address"
    else
        log_error "force_sig_seccomp address not found or is zero"
        exit 1
    fi

    if [ "$fcesig_ptrace_errno_trap_address" != "0000000000000000" ] && [ -n "$fcesig_ptrace_errno_trap_address" ]; then
        log_info "force_sig_ptrace_errno_trap address: $fcesig_ptrace_errno_trap_address"
    else
        log_error "force_sig_ptrace_errno_trap address not found or is zero"
        exit 1
    fi

    if [ "$force_sig_fault_trapno_address" != "0000000000000000" ] && [ -n "$force_sig_fault_trapno_address" ]; then
        log_info "force_sig_fault_trapno address: $force_sig_fault_trapno_address"
    else
        log_error "force_sig_fault_trapno address not found or is zero"
        exit 1
    fi

    if [ "$send_sig_fault_trapno_address" != "0000000000000000" ] && [ -n "$send_sig_fault_trapno_address" ]; then
        log_info "send_sig_fault_trapno address: $send_sig_fault_trapno_address"
    else
        log_error "send_sig_fault_trapno address not found or is zero"
        exit 1
    fi
}

test_mod() {
    insmod $test_ko addr1=0x$force_sig_seccomp_address addr2=0x$fcesig_ptrace_errno_trap_address addr3=0x$force_sig_fault_trapno_address \
    addr4=0x$send_sig_fault_trapno_address
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
