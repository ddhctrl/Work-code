#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 信号处理函数
void signal_handler(int sig) {
    printf("Caught signal %d\n", sig);
}

int main() {
    pid_t pid;
    int sig;

    // 获取当前进程的 PID
    pid = getpid();
    // 设置信号为 SIGUSR1
    sig = SIGUSR1;

    // 设置信号处理函数
    if (signal(sig, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Sending signal %d to process %d\n", sig, pid);

    // 使用标准库中的 kill 函数发送信号
    if (kill(pid, sig) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("Signal sent successfully\n");

    return 0;
}
