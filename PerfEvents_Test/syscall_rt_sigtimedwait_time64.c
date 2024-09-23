#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define __NR_rt_sigtimedwait_time64 421

struct __kernel_timespec {
    long tv_sec;
    long tv_nsec;
};

int main() {
    sigset_t set;
    siginfo_t info;
    struct __kernel_timespec timeout;
    long ret;

    // 初始化信号集
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    // 设置超时时间为5秒
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;

    // 调用系统调用
    syscall(__NR_rt_sigtimedwait_time64, &set, &info, &timeout, sizeof(sigset_t));

    if (ret == -1) {
        perror("rt_sigtimedwait_time64");
        exit(EXIT_FAILURE);
    }

    printf("Caught signal %d\n", info.si_signo);
    return 0;
}
