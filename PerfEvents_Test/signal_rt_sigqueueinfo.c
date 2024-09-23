#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef SYS_rt_sigqueueinfo
#define SYS_rt_sigqueueinfo 138 // 需要根据系统架构查找具体的系统调用号
#endif

int rt_sigqueueinfo(pid_t pid, int sig, siginfo_t *uinfo) {
    return syscall(SYS_rt_sigqueueinfo, pid, sig, uinfo);
}

void signal_handler(int sig, siginfo_t *info, void *context) {
    printf("Received signal %d with code %d and value %d\n",
           sig, info->si_code, info->si_value.sival_int);
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    pid_t pid = getpid();
    int sig = SIGUSR1;
    siginfo_t uinfo;
    memset(&uinfo, 0, sizeof(uinfo));
    uinfo.si_value.sival_int = 42;

    if (rt_sigqueueinfo(pid, sig, &uinfo) == -1) {
        perror("rt_sigqueueinfo");
        exit(EXIT_FAILURE);
    }

    printf("Signal sent\n");

    return 0;
}
