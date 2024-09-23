#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <signal.h>
#include <asm/unistd.h>

static int fd;
static long long count;
static volatile int event_paused = 0; // 标志表示事件是否被挂起

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

void handle_signal(int signal, siginfo_t *si, void *context) {
    if (signal == SIGUSR1) {
        if (event_paused) {
            // 恢复事件
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
            event_paused = 0;
            printf("Event resumed\n");
        } else {
            // 挂起事件
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            read(fd, &count, sizeof(long long));
            printf("SIGUSR1 received, used %lld instructions\n", count);
            event_paused = 1;
        }
    }
}

int main() {
    struct perf_event_attr pe;

    // 初始化 perf_event_attr 结构体
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = PERF_ATTR_SIZE_VER5;
    pe.config = PERF_COUNT_HW_CPU_CYCLES;  // 监控指令计数
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.sample_period = 1000000;  // 设置采样周期，触发信号
    pe.wakeup_events = 1;        // 设置唤醒事件数

    // 打开性能事件
    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    // 设置信号处理函数
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    // 启动计数
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 模拟长时间运行的代码块，并在特定时间点发送信号
    for (long i = 0; i < 1000000000; i++) {
        if (i % 100000000 == 0) {
            printf("Progress: %ld\n", i);
            // 发送 SIGUSR1 信号
            kill(getpid(), SIGUSR1);
        }
    }

    // 停止计数
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &count, sizeof(long long));

    printf("Final count: Used %lld instructions\n", count);

    // 关闭文件描述符
    close(fd);

    return 0;
}
