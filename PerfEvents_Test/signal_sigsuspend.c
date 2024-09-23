#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

void signal_handler(int sig) {
    printf("Received signal %d\n", sig);
}

int main() {
    sigset_t mask, oldmask;

    // 初始化信号集，添加 SIGUSR1
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    // 安装信号处理程序
    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }

    if (signal(SIGALRM, signal_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }

    // 阻塞 SIGUSR1 并保存旧的信号屏蔽字
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    printf("SIGUSR1 blocked. Waiting for signal...\n");

    alarm(10);

    // 使用 sigsuspend 挂起进程，等待信号
    sigset_t waitmask;
    sigemptyset(&waitmask);  // sigsuspend 期间不阻塞任何信号

    // 挂起进程，等待信号
    if (sigsuspend(&waitmask) == -1) {
        perror("sigsuspend");
    }

    // 恢复旧的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    printf("Exited sigsuspend.\n");
    return EXIT_SUCCESS;
}
