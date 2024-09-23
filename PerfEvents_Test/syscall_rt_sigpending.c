#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void signal_handler(int sig) {
    printf("Signal %d received, but will be blocked\n", sig);
}

int main() {
    sigset_t new_set, old_set, pending_set;

    // 初始化信号集，将 SIGUSR1 添加到新信号集中
    sigemptyset(&new_set);
    sigaddset(&new_set, SIGUSR1);

    // 阻塞 SIGUSR1 信号
    if (sigprocmask(SIG_BLOCK, &new_set, &old_set) == -1) {
        perror("sigprocmask");
        return 1;
    }

    // 设置信号处理程序
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    // 发送 SIGUSR1 信号，使其挂起
    printf("Sending SIGUSR1 to self\n");
    if (kill(getpid(), SIGUSR1) == -1) {
        perror("kill");
        return 1;
    }

    // 检查挂起的信号
    if (sigpending(&pending_set) == -1) {
        perror("sigpending");
        return 1;
    }

    printf("Pending signals:\n");
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(&pending_set, i)) {
            printf("Signal %d is pending\n", i);
        }
    }

    // 恢复之前的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &old_set, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }

    return 0;
}
