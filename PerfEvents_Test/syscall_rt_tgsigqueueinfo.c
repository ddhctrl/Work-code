#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

    pid_t tgid = getpid();      // 获取当前进程ID
    pid_t pid = syscall(SYS_gettid); // 获取当前线程ID
    int sig = SIGUSR1;
    siginfo_t uinfo;
    memset(&uinfo, 0, sizeof(uinfo));
    uinfo.si_value.sival_int = 42;

    if (syscall(__NR_rt_tgsigqueueinfo, tgid, pid, sig, &uinfo) == -1) {
        perror("rt_tgsigqueueinfo");
        exit(EXIT_FAILURE);
    }

    printf("Signal %d sent to thread %d of process %d successfully\n", sig, pid, tgid);

    return 0;
}
