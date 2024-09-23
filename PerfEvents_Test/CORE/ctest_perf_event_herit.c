#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/wait.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main(int argc, char **argv) {
    struct perf_event_attr pe;
    int fd;
    long long count;

    // 配置性能事件属性
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.inherit = 1; // 子进程继承性能事件
    pe.exclude_kernel = 0;
    pe.exclude_hv = 0;

    // 打开性能事件
    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    // 开始计数
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程
        for (int i = 0; i < 1000000; i++) {
            asm volatile("" ::: "memory");
        }
        // 读取子进程计数值
        read(fd, &count, sizeof(long long));
        printf("Child CPU cycles: %lld\n", count);
    } else {
        // 父进程
        wait(NULL); // 等待子进程结束
        // 读取父进程计数值
        read(fd, &count, sizeof(long long));
        printf("Parent CPU cycles: %lld\n", count);
    }

    // 停止计数
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    // 关闭文件描述符
    close(fd);

    return 0;
}
