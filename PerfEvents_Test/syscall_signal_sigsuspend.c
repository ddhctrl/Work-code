#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdint.h>

typedef uint32_t old_sigset_t;

void signal_handler(int sig) {
    printf("Received signal %d\n", sig);
}

int main() {
    sigset_t newset, oldset;

    // 初始化新的信号集，添加 SIGUSR1
    sigemptyset(&newset);
    sigaddset(&newset, SIGUSR1);

    // 安装信号处理程序
    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }

    // 阻塞 SIGUSR1 信号
    if (sigprocmask(SIG_BLOCK, &newset, &oldset) == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    printf("SIGUSR1 blocked. Waiting for signal...\n");

    alarm(5);

    // 使用 syscall 调用 sigsuspend 系统调用
    old_sigset_t mask = 1 << (SIGUSR1 - 1);
    if (syscall(__NR_sigsuspend, 0, 0, mask) == -1 && errno != EINTR) {
        perror("syscall sigsuspend");
    }

    // 恢复旧的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldset, NULL) == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }


    printf("Exited sigsuspend.\n");
    return EXIT_SUCCESS;
}
