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
#include <errno.h>
#include <inttypes.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

void run_task() {
    for (int i = 0; i < 1000000; i++) {
        asm volatile("" ::: "memory");
    }
}

int main(int argc, char **argv) {
    struct perf_event_attr pe[4];
    int fd, child_fd;
    long long count;
    pid_t pid;
    int id[5];

    // 配置父进程的性能事件属性
    memset(&pe[0], 0, sizeof(struct perf_event_attr));
    pe[0].type = PERF_TYPE_HARDWARE;
    pe[0].size = sizeof(struct perf_event_attr);
    pe[0].config = PERF_COUNT_HW_CPU_CYCLES;
    pe[0].disabled = 1;
    pe[0].inherit = 1; // 子进程继承性能事件
    pe[0].exclude_kernel = 1;
    pe[0].exclude_hv = 1;

    // 打开父进程的性能事件
    fd = perf_event_open(&pe[0], 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening parent event %llx: %s\n", pe[0].config, strerror(errno));
        exit(EXIT_FAILURE);
    }
    ioctl(fd, PERF_EVENT_IOC_ID, &id[0]);

    // 开始计数
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 创建多个子进程来增加系统负载
    for (int i = 1; i < 5; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // 子进程配置不同的性能事件属性
            memset(&pe[i], 0, sizeof(struct perf_event_attr));
            pe[i].type = PERF_TYPE_HARDWARE;
            pe[i].size = sizeof(struct perf_event_attr);
            pe[i].config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
            pe[i].exclude_kernel = 1;
            pe[i].exclude_hv = 1;

            // 打开子进程的性能事件
            child_fd = perf_event_open(&pe[i], 0, -1, -1, 0);
            if (child_fd == -1) {
                fprintf(stderr, "Error opening child event %llx: %s\n", pe[i].config, strerror(errno));
                exit(EXIT_FAILURE);
            }
            ioctl(child_fd, PERF_EVENT_IOC_ID, &id[i]);

            ioctl(child_fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(child_fd, PERF_EVENT_IOC_ENABLE, 0);

            // 运行高负载任务
            run_task();

            // 读取子进程计数值
            read(child_fd, &count, sizeof(long long));
            printf("Child %d stalled cycles: %lld\n", i, count);

            ioctl(child_fd, PERF_EVENT_IOC_DISABLE, 0);
            close(child_fd);

            exit(EXIT_SUCCESS);
        }
    }

    // 父进程运行高负载任务
    run_task();

    // 等待所有子进程结束
    while (wait(NULL) > 0);

    // 读取父进程计数值
    read(fd, &count, sizeof(long long));
    printf("Parent CPU cycles: %lld\n", count);

    // 停止计数
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    // 关闭文件描述符
    close(fd);

    return 0;
}
