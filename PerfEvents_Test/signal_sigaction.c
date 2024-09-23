#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h> 

typedef uint32_t compat_uptr_t;
typedef uint32_t compat_old_sigset_t;
typedef uint32_t compat_ulong_t;

struct compat_old_sigaction {
    compat_uptr_t sa_handler;
    compat_old_sigset_t sa_mask;
    compat_ulong_t sa_flags;
    compat_uptr_t sa_restorer;
};

#define SIGUSR1 10

void signal_handler(int sig) {
    printf("Received signal %d\n", sig);
}

int main() {
    struct compat_old_sigaction new_action, old_action;
    int result;

    // 初始化新的信号处理行为
    new_action.sa_handler = (compat_uptr_t)(uintptr_t)signal_handler;
    new_action.sa_mask = 0;
    new_action.sa_flags = 0;
    new_action.sa_restorer = (compat_uptr_t)(uintptr_t)NULL;

    // 调用 sigaction 系统调用来设置信号处理程序
    result = syscall(__NR_sigaction, SIGUSR1, &new_action, &old_action);
    if (result == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // 发送信号给自身，使用 tkill 系统调用
    result = syscall(__NR_tkill, getpid(), SIGUSR1);
    if (result == -1) {
        perror("tkill");
        return EXIT_FAILURE;
    }

    // 恢复原来的信号处理行为
    result = syscall(__NR_sigaction, SIGUSR1, &old_action, NULL);
    if (result == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
