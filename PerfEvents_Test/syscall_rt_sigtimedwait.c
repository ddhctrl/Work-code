#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void catcher(int sig) {
    printf("Signal catcher called for signal %d\n", sig);
}

void timestamp(char *str) {
    time_t t;
    time(&t);
    printf("The time %s is %s\n", str, ctime(&t));
}

int main() {
    sigset_t set;
    siginfo_t info;
    struct timespec timeout;
    int result;

    // 初始化信号集，添加需要等待的信号
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    // 阻塞 SIGUSR1 信号，防止其被默认处理
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }

    // 设置超时时间为5秒
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;

    // 创建一个子进程，在子进程中发送 SIGUSR1 信号
    if (fork() == 0) {
        // 子进程
        sleep(4); // 等待1秒
        kill(getppid(), SIGUSR1); // 向父进程发送 SIGUSR1
        return 0;
    }

    // 打印时间戳
    timestamp("before sigtimedwait()");

    // 调用 sigtimedwait 等待信号
    result = sigtimedwait(&set, &info, &timeout);
    if (result == -1) {
        if (errno == EAGAIN) {
            printf("Timeout occurred, no signal received.\n");
        } else {
            perror("sigtimedwait");
        }
        return 1;
    } else {
        printf("sigtimedwait() returned for signal %d, ret:%d\n", info.si_signo, result);
    }

    // 打印时间戳
    timestamp("after sigtimedwait()");

    return 0;
}
