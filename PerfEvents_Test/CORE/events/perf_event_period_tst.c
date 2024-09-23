#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdint.h>

// 定义 PERF_EVENT_IOC_PERIOD 命令
#ifndef PERF_EVENT_IOC_PERIOD
#define PERF_EVENT_IOC_PERIOD _IOW('$', 0, uint64_t)
#endif

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main()
{
    struct perf_event_attr pe;
    int fd;
    uint64_t period = 100000; // 新的周期值
    uint64_t count;

    // 初始化 perf_event_attr 结构
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.sample_period = period;

    // 打开 perf 事件
    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    // 设置事件周期
    if (ioctl(fd, PERF_EVENT_IOC_PERIOD, &period) == -1) {
        fprintf(stderr, "Error setting period\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 启用事件
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 模拟一些工作负载
    for (volatile int i = 0; i < 100000000; i++);

    // 读取事件计数值
    if (read(fd, &count, sizeof(uint64_t)) == -1) {
        fprintf(stderr, "Error reading count\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Count: %llu\n", (unsigned long long)count);

    // 停止和关闭事件
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    close(fd);

    return 0;
}

