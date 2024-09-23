#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>

// The function to count through (called in main)
void code_to_measure(){
    int sum = 0;
    for(int i = 0; i < 1000000000; ++i){
        sum += 1;
    }
}

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct perf_event_attr pe1, pe2, pe3, pe4;
    int fd1, fd2, fd3, fd4, cgroup_fd1, cgroup_fd2, cgroup_fd3, cgroup_fd4;
    uint64_t val1, val2, val3, val4;

    // 打开 cgroup 文件描述符
    cgroup_fd1 = open("/sys/fs/cgroup", O_RDONLY);
    if (cgroup_fd1 == -1) {
        perror("open cgroup1");
        exit(EXIT_FAILURE);
    }
    printf("cgroup fd1: %d\n", cgroup_fd1);

    cgroup_fd2 = open("/sys/fs/cgroup/L1", O_RDONLY);
    if (cgroup_fd2 == -1) {
        perror("open cgroup2");
        exit(EXIT_FAILURE);
    }
    printf("cgroup fd2: %d\n", cgroup_fd2);

    cgroup_fd3 = open("/sys/fs/cgroup/L1/L2", O_RDONLY);
    if (cgroup_fd3 == -1) {
        perror("open cgroup3");
        exit(EXIT_FAILURE);
    }
    printf("cgroup fd3: %d\n", cgroup_fd3);

    cgroup_fd4 = open("/sys/fs/cgroup/L1/L2/L3", O_RDONLY);
    if (cgroup_fd4 == -1) {
        perror("open cgroup4");
        exit(EXIT_FAILURE);
    }
    printf("cgroup fd4: %d\n", cgroup_fd4);

    // 配置第一个 perf_event_attr 结构体
    memset(&pe1, 0, sizeof(struct perf_event_attr));
    pe1.type = PERF_TYPE_HARDWARE;
    pe1.size = sizeof(struct perf_event_attr);
    pe1.config = PERF_COUNT_HW_CPU_CYCLES;
    pe1.sample_period = 6000;  // 设置采样周期
    pe1.sample_type = PERF_SAMPLE_IP;
    pe1.disabled = 1;
    pe1.exclude_kernel = 1;
    pe1.exclude_hv = 1;
    pe1.read_format = PERF_FORMAT_GROUP;

    // 配置第二个 perf_event_attr 结构体
    memset(&pe2, 0, sizeof(struct perf_event_attr));
    pe2.type = PERF_TYPE_HARDWARE;
    pe2.size = sizeof(struct perf_event_attr);
    pe2.config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
    pe2.sample_period = 6000;  // 设置采样周期
    pe2.sample_type = PERF_SAMPLE_IP;
    pe2.disabled = 1;
    pe2.exclude_kernel = 1;
    pe2.exclude_hv = 1;
    pe2.read_format = PERF_FORMAT_GROUP;

    // 配置第三个 perf_event_attr 结构体
    memset(&pe3, 0, sizeof(struct perf_event_attr));
    pe3.type = PERF_TYPE_SOFTWARE;
    pe3.size = sizeof(struct perf_event_attr);
    pe3.config = PERF_COUNT_SW_TASK_CLOCK;
    pe3.sample_period = 6000;  // 设置采样周期
    pe3.sample_type = PERF_SAMPLE_IP;
    pe3.disabled = 1;
    pe3.exclude_kernel = 1;
    pe3.exclude_hv = 1;
    pe3.read_format = PERF_FORMAT_GROUP;

    // 配置第四个 perf_event_attr 结构体
    memset(&pe4, 0, sizeof(struct perf_event_attr));
    pe4.type = PERF_TYPE_SOFTWARE;
    pe4.size = sizeof(struct perf_event_attr);
    pe4.config = PERF_COUNT_SW_PAGE_FAULTS;
    pe4.sample_period = 6000;  // 设置采样周期
    pe4.sample_type = PERF_SAMPLE_IP;
    pe4.disabled = 1;
    pe4.exclude_kernel = 1;
    pe4.exclude_hv = 1;
    pe4.read_format = PERF_FORMAT_GROUP;

    // 创建第一个性能事件
    fd1 = perf_event_open(&pe1, cgroup_fd1, 0, -1, PERF_FLAG_PID_CGROUP);
    if (fd1 == -1) {
        fprintf(stderr, "Error opening leader %llx: %s\n", pe1.config, strerror(errno));
        close(cgroup_fd1);
        exit(EXIT_FAILURE);
    }
    printf("open event fd1: %d\n", fd1);

    // 创建第二个性能事件，作为第一个事件的子事件
    fd2 = perf_event_open(&pe2, cgroup_fd2, 0, -1, PERF_FLAG_PID_CGROUP);
    if (fd2 == -1) {
        fprintf(stderr, "Error opening event %llx: %s\n", pe2.config, strerror(errno));
        close(fd1);
        close(cgroup_fd2);
        exit(EXIT_FAILURE);
    }
    printf("open event fd2: %d\n", fd2);

    // 创建第三个性能事件，作为第一个事件的子事件
    fd3 = perf_event_open(&pe3, cgroup_fd3, 0, -1, PERF_FLAG_PID_CGROUP);
    if (fd3 == -1) {
        fprintf(stderr, "Error opening event %llx: %s\n", pe3.config, strerror(errno));
        close(fd2);
        close(fd1);
        close(cgroup_fd3);
        exit(EXIT_FAILURE);
    }
    printf("open event fd3: %d\n", fd3);

    // 创建第四个性能事件，作为第一个事件的子事件
    fd4 = perf_event_open(&pe4, cgroup_fd4, 0, -1, PERF_FLAG_PID_CGROUP);
    if (fd4 == -1) {
        fprintf(stderr, "Error opening event %llx: %s\n", pe4.config, strerror(errno));
        close(fd3);
        close(fd2);
        close(fd1);
        close(cgroup_fd4);
        exit(EXIT_FAILURE);
    }
    printf("open event fd4: %d\n", fd4);

    // Reset counters and start counting
    printf("ioctl fd1\n");
    ioctl(fd1, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd1, PERF_EVENT_IOC_ENABLE, 0);
    printf("ioctl fd2\n");
    ioctl(fd2, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd2, PERF_EVENT_IOC_ENABLE, 0);
    printf("ioctl fd3\n");
    ioctl(fd3, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd3, PERF_EVENT_IOC_ENABLE, 0);
    printf("ioctl fd4\n");
    ioctl(fd4, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd4, PERF_EVENT_IOC_ENABLE, 0);

    // Example code to count through
    code_to_measure();

    // Stop counting
    printf("ioctl disable fd1\n");
    ioctl(fd1, PERF_EVENT_IOC_DISABLE, 0);
    printf("ioctl disable fd2\n");
    ioctl(fd2, PERF_EVENT_IOC_DISABLE, 0);
    printf("ioctl disable fd3\n");
    ioctl(fd3, PERF_EVENT_IOC_DISABLE, 0);
    printf("ioctl disable fd4\n");
    ioctl(fd4, PERF_EVENT_IOC_DISABLE, 0);

    // Read and print results
    printf("read fd1\n");
    read(fd1, &val1, sizeof(val1));
    printf("read fd2\n");
    read(fd2, &val2, sizeof(val2));
    printf("read fd3\n");
    read(fd3, &val3, sizeof(val3));
    printf("read fd4\n");
    read(fd4, &val4, sizeof(val4));
    printf("CPU cycles: %"PRIu64"\n", val1);
    printf("Instructions retired: %"PRIu64"\n", val2);
    printf("Task clock: %"PRIu64"\n", val3);
    printf("Page faults: %"PRIu64"\n", val4);

    // 触发 _free_event
    close(fd4);
    close(fd3);
    close(fd2);
    close(fd1);
    close(cgroup_fd1);
    close(cgroup_fd2);
    close(cgroup_fd3);
    close(cgroup_fd4);

    return 0;
}