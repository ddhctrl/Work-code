#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int get_tracepoint_id(const char *tracepoint) {
    char path[256];
    char id_str[16];
    int fd, id;

    snprintf(path, sizeof(path), "/sys/kernel/debug/tracing/events/%s/id", tracepoint);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open tracepoint ID file");
        return -1;
    }

    if (read(fd, id_str, sizeof(id_str)) < 0) {
        perror("Failed to read tracepoint ID");
        close(fd);
        return -1;
    }

    close(fd);
    id = atoi(id_str);
    return id;
}

int main(int argc, char **argv) {
    struct perf_event_attr pe;
    long long count;
    int fd;
    const char *tracepoint = "sched/sched_switch";
    const char *filter = NULL; // 不使用过滤器
    int tracepoint_id;

    // 获取 sched:sched_switch 事件的 ID
    tracepoint_id = get_tracepoint_id(tracepoint);
    if (tracepoint_id < 0) {
        fprintf(stderr, "Error getting tracepoint ID for %s\n", tracepoint);
        exit(EXIT_FAILURE);
    }

    // 初始化 perf_event_attr 结构体
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_TRACEPOINT;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = tracepoint_id;
    pe.disabled = 1;
    pe.exclude_kernel = 0;
    pe.exclude_hv = 0;

    // 调用 perf_event_open
    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx: %s\n", pe.config, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // 设置过滤器（如果需要）
    if (filter && ioctl(fd, PERF_EVENT_IOC_SET_FILTER, filter) == -1) {
        fprintf(stderr, "Error setting filter: %s\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 启动计数器
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 被监控的代码区域
    for (volatile int i = 0; i < 100000000; i++);

    // 停止计数器
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &count, sizeof(long long));

    // 输出计数结果
    printf("Tracepoint events: %lld\n", count);

    // 关闭文件描述符
    close(fd);

    return 0;
}

