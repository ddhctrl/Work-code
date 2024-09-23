#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

void signal_handler(int sig) {
    printf("Timer sig\n");
}

void set_rlimit_sigpending(int new_limit) {
    struct rlimit limit;
    limit.rlim_cur = new_limit;
    limit.rlim_max = new_limit;
    if (setrlimit(RLIMIT_SIGPENDING, &limit) == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }
}

void send_signals(pid_t pid, int count) {
    for (int i = 0; i < count; i++) {
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill");
            break;
        }
    }
}

int main() {
    timer_t timerid;
    struct sigevent sev;
    struct sigaction sa;
    pid_t pid = getpid();
    int sig = SIGUSR1;

    // 将 RLIMIT_SIGPENDING 设置为一个较低的值以便于测试
    set_rlimit_sigpending(1);

    // 设置信号处理程序
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(sig, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // 阻塞 SIGUSR1 信号，防止其被默认处理
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    // 发送 SIGUSR1 信号达到 RLIMIT_SIGPENDING 限制
    send_signals(pid, 2);

    printf("Sent SIGUSR1 signals\n");

    // 设置 sigevent 结构体
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN; // 用于定时器信号
    sev.sigev_value.sival_ptr = &timerid;

    // 尝试创建定时器，此时 should trigger the condition to be false
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) != 0) {
        perror("timer_create failed");
    } else {
        printf("Timer ID is %p\n", timerid);
    }

    return 0;
}
