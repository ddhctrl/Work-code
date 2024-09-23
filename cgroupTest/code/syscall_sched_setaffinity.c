#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    cpu_set_t mask;
    int num_cpus;
    int cpu;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <pid> <cpu>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);
    cpu = atoi(argv[2]);

    // 获取系统中的CPU数量
    num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu < 0 || cpu >= num_cpus) {
        fprintf(stderr, "Invalid CPU number. The CPU number should be between 0 and %d.\n", num_cpus - 1);
        exit(EXIT_FAILURE);
    }

    // 初始化 CPU 集合
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);

    // 调用 sched_setaffinity 系统调用设置 CPU 亲和性
    if (sched_setaffinity(pid, sizeof(mask), &mask) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    printf("Successfully set CPU affinity for process %d to CPU %d\n", pid, cpu);

    return 0;
}
