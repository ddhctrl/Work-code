#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>

#define __NR_pidfd_send_signal 424

// 信号处理程序
void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    printf("Caught signal %d, with value %d\n", signo, info->si_value.sival_int);
}

int main() {
    pid_t target_pid = getpid();
    int pidfd;
    int sig = SIGUSR1;
    siginfo_t info;
    unsigned int flags = 0;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // 打开目标进程的 pidfd
    pidfd = syscall(SYS_pidfd_open, target_pid, 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        return EXIT_FAILURE;
    }

    // 初始化 siginfo_t 结构体
    info.si_signo = sig;
    info.si_code = SI_QUEUE;
    info.si_value.sival_int = 1234;  // 可以传递附加信息

    // 调用 pidfd_send_signal 系统调用
    if (syscall(__NR_pidfd_send_signal, pidfd, sig, &info, flags) == -1) {
        perror("pidfd_send_signal");
        close(pidfd);
        return EXIT_FAILURE;
    }

    printf("Signal sent successfully\n");

    // 等待信号处理
    sleep(1);

    // 关闭 pidfd
    close(pidfd);

    return EXIT_SUCCESS;
}
