#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

unsigned long addr1 = 0; 
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
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx\n", pe.config);
        exit(EXIT_FAILURE);
    }

    printf("perf_event_open fd: %d\n", fd);

    FILE *fp = popen("cat /proc/kallsyms | grep perf_pmu_resched", "r");
    if (fp == NULL) {
        perror("popen");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (fscanf(fp, "%lx", &addr1) != 1) {
        fprintf(stderr, "Error reading address\n");
        pclose(fp);
        close(fd);
        exit(EXIT_FAILURE);
    }
    pclose(fp);
    printf("Setting addr1: 0x%lx\n", addr1);

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "insmod ko_perf_pmu_resched.ko addr1=0x%lx fd=%d", addr1, fd);
    system(cmd);

    // 关闭文件描述符
    close(fd);

     // 删除内核模块
    snprintf(cmd, sizeof(cmd), "sudo rmmod ko_perf_pmu_resched");
    printf("Executing command: %s\n", cmd);
    system(cmd);

    return 0;
}
