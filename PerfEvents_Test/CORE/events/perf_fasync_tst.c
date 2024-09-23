#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

// 异步信号处理函数
volatile sig_atomic_t signal_count = 0;
volatile sig_atomic_t received_signal = 0;

void signal_handler(int signum, siginfo_t *info, void *context) {
    signal_count++;
    received_signal = 1;
    printf("Received signal %d from kernel\n", signum);
    if (info && info->si_code == POLL_HUP) {
        printf("Signal info: POLL_HUP received\n");
    } else {
        printf("Signal info: code %d\n", info ? info->si_code : -1);
    }
}

int main()
{
    int fd;
    struct perf_event_attr pe;
    struct sigaction sa;

    // 初始化信号处理
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // 初始化 perf_event_attr 结构
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.sample_period = 3000;  // 设置采样周期
    pe.sample_type = PERF_SAMPLE_IP;

    // 使用 perf_event_open 打开 perf 事件
    fd = perf_event_open(&pe, -1, 0, -1, 0);
    if (fd == -1) {
        perror("perf_event_open");
        exit(EXIT_FAILURE);
    }

    // 设置文件描述符的异步通知标志
    if (fcntl(fd, F_SETFL, O_ASYNC | fcntl(fd, F_GETFL)) == -1) {
        perror("fcntl F_SETFL");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 设置异步通知的所有者为当前进程
    if (fcntl(fd, F_SETOWN, getpid()) == -1) {
        perror("fcntl F_SETOWN");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 启用事件
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 等待异步信号
    printf("Waiting for signals...\n");
    while (signal_count < 5) {
        if (received_signal) {
            received_signal = 0;
            // 处理信号后的逻辑
        }
    }

    printf("Received 10 signals, exiting...\n");

    // 关闭文件
    close(fd);

    return 0;
}

