#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>

void signal_handler(int sig) {
    printf("Received signal %d\n", sig);
}

int main() {
    sigset_t pending_set;
    sigset_t block_set;

    // 安装信号处理程序
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    // 阻塞 SIGUSR1 和 SIGUSR2 信号
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    sigaddset(&block_set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &block_set, NULL);

    // 发送信号给自身
    raise(SIGUSR1);
    raise(SIGUSR2);

    // 调用 sigpending 系统调用
    if (syscall(__NR_sigpending, &pending_set) == -1) {
        perror("sigpending");
        return EXIT_FAILURE;
    }

    // 检查并打印挂起的信号
    for (int sig = 1; sig < NSIG; sig++) {
        if (sigismember(&pending_set, sig)) {
            printf("Signal %d is pending\n", sig);
        }
    }
    printf("Signal pending finish!\n");

    return EXIT_SUCCESS;
}
