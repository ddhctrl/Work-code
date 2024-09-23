#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define __NR_tkill 130

// 信号处理程序
void sigusr1_handler(int signo) {
    printf("Caught signal %d\n", signo);
}

int main() {
    pid_t target_tid = getpid();  // 获取当前线程的TID
    int sig = SIGUSR1;

    // 设置 SIGUSR1 信号处理程序
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // 调用 tkill 系统调用
    if (syscall(__NR_tkill, target_tid, sig) == -1) {
        perror("tkill");
        return EXIT_FAILURE;
    }

    // 暂停，以等待信号处理程序运行
    sleep(2);

    return EXIT_SUCCESS;
}