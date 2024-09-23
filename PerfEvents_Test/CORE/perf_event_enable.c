#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define ARMV8_PMUV3_PERFCTR_CPU_CYCLES 0x11

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
}

int main(int argc, char **argv)
{
    struct perf_event_attr pe;
    int fd;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_RAW;
    pe.size = sizeof(struct perf_event_attr);
    pe.pinned = 1;
    pe.disabled = 0; // Event is initially enabled
    pe.exclude_user = 0;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.exclude_host = 1;
    pe.config = ARMV8_PMUV3_PERFCTR_CPU_CYCLES;

    fd = perf_event_open(&pe, -1, 0, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    printf("perf_event_open fd: %d\n", fd);

    // 传递文件描述符到内核模块
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "sudo insmod ko_perf_event_enable.ko fd=%d", fd);
    printf("Executing command: %s\n", cmd);
    system(cmd);

    // 关闭文件描述符
    close(fd);

    // 删除内核模块
    snprintf(cmd, sizeof(cmd), "sudo rmmod ko_perf_event_enable");
    printf("Executing command: %s\n", cmd);
    system(cmd);

    return 0;
}
