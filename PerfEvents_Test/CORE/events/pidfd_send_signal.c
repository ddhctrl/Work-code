#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <errno.h>
#include <stdbool.h>

volatile sig_atomic_t signal_handled = false;

void signal_handler(int signum) {
    printf("Received signal: %d\n", signum);
    signal_handled = true;  // 设置标志位，表示信号已处理
}

int main() {
    pid_t pid = getpid();  // 获取当前进程ID
    int pidfd;

    // 设置信号处理程序
    signal(SIGUSR1, signal_handler);

    printf("Process ID: %d\n", pid);

    // 打开pidfd
    pidfd = syscall(SYS_pidfd_open, pid, 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        exit(EXIT_FAILURE);
    }

    // 使用pidfd_send_signal发送信号
    if (syscall(SYS_pidfd_send_signal, pidfd, SIGUSR1, NULL, 0) == -1) {
        perror("pidfd_send_signal");
        close(pidfd);
        exit(EXIT_FAILURE);
    }

    printf("Signal sent using pidfd_send_signal.\n");

    // 等待信号处理完成
    while (!signal_handled) {
        pause();
    }

    printf("Signal handled, exiting program.\n");

    close(pidfd);
    return 0;
}

