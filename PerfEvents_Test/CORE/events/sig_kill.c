#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdbool.h>

volatile sig_atomic_t signal_count = 0;
const int total_signals = 3;

void signal_handler(int signum) {
    printf("Received signal: %d\n", signum);
    signal_count++;

    if (signal_count >= total_signals) {
        printf("All signals handled, exiting program.\n");
        exit(0);
    }
}

int main() {
    pid_t pid = getpid();       // 获取当前进程ID
    pid_t tid = syscall(SYS_gettid); // 获取当前线程ID

    // 设置信号处理程序
    signal(SIGUSR1, signal_handler);

    printf("Process ID: %d\n", pid);
    printf("Thread ID: %ld\n", (long)tid);

    // 使用 kill 发送信号
    printf("Sending signal using kill...\n");
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    // 使用 tkill 发送信号
    printf("Sending signal using tkill...\n");
    if (syscall(SYS_tkill, tid, SIGUSR1) == -1) {
        perror("tkill");
        exit(EXIT_FAILURE);
    }

    // 使用 tgkill 发送信号
    printf("Sending signal using tgkill...\n");
    if (syscall(SYS_tgkill, pid, tid, SIGUSR1) == -1) {
        perror("tgkill");
        exit(EXIT_FAILURE);
    }

    // 等待信号处理
    while (true) {
        pause(); // 等待信号，直到所有信号处理完毕
    }

    return 0;
}

