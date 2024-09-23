#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

timer_t timerid;
int trigger_count = 0;

void timer_handler(int sig, siginfo_t *si, void *uc) {
    printf("Timer expired! Timer ID: %p\n", si->si_value.sival_ptr);
    trigger_count++;

    if (trigger_count >= 5) {
        printf("10 seconds passed. Exiting...\n");
        
        // 删除定时器
        if (timer_delete(timerid) == -1) {
            perror("timer_delete");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
}

void stop_timer(int sig) {
    // 停止定时器
    struct itimerspec its;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    // 删除定时器
    if (timer_delete(timerid) == -1) {
        perror("timer_delete");
        exit(EXIT_FAILURE);
    }

    printf("Timer stopped and deleted\n");
    exit(EXIT_SUCCESS);
}

int main() {
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;
    int timer_sig = SIGRTMIN;

    // 设置信号处理程序
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(timer_sig, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // 设置 SIGINT 信号处理程序，用于停止定时器
    signal(SIGINT, stop_timer);

    // 设置 sigevent 结构体
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = timer_sig;
    sev.sigev_value.sival_ptr = &timerid;

    // 创建定时器
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    printf("Timer ID is %p\n", timerid);

    // 设置定时器时间：2秒后触发，然后每隔2秒触发一次
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 2;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    // 等待信号处理
    while (1) {
        pause(); // 等待信号
    }

    return 0;
}
