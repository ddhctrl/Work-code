#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>      // 包含此头文件以定义 CLOCK_MONOTONIC
#include <stdint.h>    // 包含此头文件以定义 uint64_t
#include <inttypes.h>  // 包含此头文件以定义 PRIu64

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    // 设置 use_clockid 和 clockid 字段
    pe.use_clockid = 1;
    pe.clockid = CLOCK_MONOTONIC;

    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        perror("perf_event_open");
        return EXIT_FAILURE;
    }

    printf("perf_event_open succeeded, fd = %d\n", fd);

    // 开启性能计数器
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 模拟一些工作负载
    for (volatile int i = 0; i < 1000000; i++);

    // 停止性能计数器
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    uint64_t count;
    read(fd, &count, sizeof(uint64_t));
    printf("CPU cycles: %" PRIu64 "\n", count);

    close(fd);
    return EXIT_SUCCESS;
}

