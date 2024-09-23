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
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <string.h>

#define ARMV8_PMUV3_PERFCTR_L1D_CACHE 0x04

// The function to counting through (called in main)
void code_to_measure(){
  int sum = 0;
    for(int i = 0; i < 1000000000; ++i){
      sum += 1;
    }
}

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}


int main(int argc, char **argv) {
    struct perf_event_attr pe;
    int fd;
    long long val;
    const char *filter = "filter ip >= 0x400000 && ip < 0x500000";

    // 配置 perf_event_attr 结构体
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_RAW;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = ARMV8_PMUV3_PERFCTR_L1D_CACHE;  // L1 data cache access
    pe.disabled = 1; // Event is initially disabled
    pe.exclude_user = 0;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.exclude_host = 1;
    // 将 pid 设置为 cgroup 文件描述符，并传递 PERF_FLAG_PID_CGROUP 以绑定 cgroup
    fd = perf_event_open(&pe, -1, 0, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx: %s\n", pe.config, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("open event fd: %d\n", fd);

    // 设置过滤器
    //ioctl(fd, PERF_EVENT_IOC_SET_FILTER, filter);
    if (ioctl(fd, PERF_EVENT_IOC_SET_FILTER, filter) == -1) {
        perror("Error setting filter");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Reset counters and start counting
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    // Example code to count through
    code_to_measure();
    // Stop counting
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    // Read and print result
    read(fd, &val, sizeof(val));
    printf("CPU cycles: %lld\n", val);

    // 触发 _free_event
    close(fd);

    return 0;
}
