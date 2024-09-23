#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <errno.h>

void custom_signal_handler(int sig) {
    printf("Received signal %d\n", sig);
}

int main() {
    // 安装自定义的信号处理程序
    if (signal(SIGUSR1, custom_signal_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }

    if (signal(SIGALRM, custom_signal_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }

    alarm(10);

    printf("Waiting for signal SIGUSR1...\n");

    // 调用 pause 系统调用
    int result = syscall(__NR_pause);
    if (result == -1) {
        perror("syscall pause");
    }

    printf("Exited pause.\n");
    return EXIT_SUCCESS;
}
