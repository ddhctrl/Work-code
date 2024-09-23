#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <errno.h>
#include <stdbool.h>

volatile sig_atomic_t signal_handled = false;

void signal_handler(int signum, siginfo_t *info, void *context) {
    printf("Received signal %d from PID %d, UID %d\n", signum, info->si_pid, info->si_uid);
    signal_handled = true;  // 设置标志位，表示信号已处理
}

int main() {
    pid_t pid = getpid();
    pid_t tid = syscall(SYS_gettid);
    struct sigaction sa;
    siginfo_t siginfo;
    int sig = SIGUSR1;

    // 设置信号处理程序
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);

    // 初始化 siginfo 结构
    siginfo.si_signo = sig;
    siginfo.si_code = SI_QUEUE;
    siginfo.si_pid = pid;
    siginfo.si_uid = getuid();

    // 使用 rt_tgsigqueueinfo 发送信号
    if (syscall(SYS_rt_tgsigqueueinfo, pid, tid, sig, &siginfo) == -1) {
        perror("rt_tgsigqueueinfo");
        exit(EXIT_FAILURE);
    }

    // 使用循环等待信号处理完成
    while (!signal_handled) {
        pause();
    }

    printf("Signal handled, continuing with the program.\n");


    return 0;
}

