#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    struct sigaction new_action, old_action;
    int sig = SIGUSR1;

    // 设置新的信号处理程序
    new_action.sa_handler = SIG_IGN;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    // 调用 rt_sigaction 系统调用
    if (sigaction(sig, &new_action, &old_action) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Signal handler for SIGUSR1 is set.\n");

    // 发送信号以测试新的处理程序
    if (kill(getpid(), SIGUSR1) == -1) {
        perror("kill");
        return EXIT_FAILURE;
    }

    // 恢复原来的信号处理程序
    if (sigaction(sig, &old_action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Signal handler for SIGUSR1 is restored to default.\n");

    if (kill(getpid(), SIGUSR1) == -1) {
        perror("kill");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
