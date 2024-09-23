#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>

typedef unsigned long old_sigset_t;

void print_sigset(old_sigset_t set) {
    for (int sig = 1; sig <= sizeof(set) * 8; sig++) {
        if (set & (1UL << (sig - 1))) {
            printf("Signal %d is blocked\n", sig);
        }
    }
}

int main() {
    old_sigset_t new_set, old_set;
    int result;

    // 初始化信号集，将 SIGUSR1 添加到新的信号集中
    new_set = 0;
    new_set |= (1UL << (SIGUSR1 - 1));

    // 调用 sigprocmask 系统调用来阻塞 SIGUSR1
    result = syscall(__NR_sigprocmask, SIG_BLOCK, &new_set, &old_set);
    if (result == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    printf("Blocked signals after SIG_BLOCK:\n");
    print_sigset(new_set);

    // 调用 sigprocmask 系统调用来解除阻塞 SIGUSR1
    result = syscall(__NR_sigprocmask, SIG_UNBLOCK, &new_set, &old_set);
    if (result == -1) {
        perror("sigprocmask");
        return EXIT_FAILURE;
    }

    printf("Blocked signals after SIG_UNBLOCK:\n");
    print_sigset(old_set);

    return EXIT_SUCCESS;
}

