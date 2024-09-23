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
#include <sys/mman.h>

// The function to counting through (called in main)
void code_to_measure() {
    int sum = 0;
    for (int i = 0; i < 1000000000; ++i) {
        sum += 1;
    }
}

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct perf_event_attr pe;
    int fd, cgroup_fd;
    void *buf;
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t mmap_size = page_size * (1 + 8); // 1 control page + 8 data pages
    struct perf_event_mmap_page *header;
    size_t stack_size = 1024; // 要采样的用户栈大小
    uint64_t val;

    // 打开 cgroup 文件描述符
    cgroup_fd = open("/sys/fs/cgroup", O_RDONLY);
    if (cgroup_fd == -1) {
        perror("open cgroup");
        exit(EXIT_FAILURE);
    }
    printf("cgroup fd: %d\n", cgroup_fd);

    // 配置 perf_event_attr 结构体
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.sample_period = 6000;  // 设置采样周期
    pe.sample_type = PERF_SAMPLE_STACK_USER;
    pe.sample_stack_user = stack_size;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.read_format = PERF_FORMAT_GROUP;

    // 将 pid 设置为 cgroup 文件描述符，并传递 PERF_FLAG_PID_CGROUP 以绑定 cgroup
    fd = perf_event_open(&pe, cgroup_fd, 1, -1, PERF_FLAG_PID_CGROUP);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx: %s\n", pe.config, strerror(errno));
        close(cgroup_fd);
        exit(EXIT_FAILURE);
    }
    printf("open event fd: %d\n", fd);

    // 设置 mmap 缓冲区
    buf = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        perror("mmap");
        close(fd);
        close(cgroup_fd);
        exit(EXIT_FAILURE);
    }
    header = (struct perf_event_mmap_page *)buf;

    // 重置计数器并开始计数
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 运行要测量的代码
    code_to_measure();

    // 停止计数
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    // 读取和解析采样数据
    uint64_t data_head = header->data_head;
    __sync_synchronize(); // 内存屏障，确保数据头已经更新
    struct perf_event_header *event;
    char *data = (char *)buf + page_size;

    while (data_head != header->data_tail) {
        event = (struct perf_event_header *)(data + (header->data_tail % (mmap_size - page_size)));
        if (event->type == PERF_RECORD_SAMPLE) {
            // 读取和解析用户栈
            char *stack_data = (char *)event + sizeof(struct perf_event_header);
            printf("Sampled user stack (first 16 bytes): ");
            for (size_t i = 0; i < 16 && i < stack_size; i++) {
                printf("%02x ", (unsigned char)stack_data[i]);
            }
            printf("\n");
        }
        header->data_tail += event->size;
    }

    // 释放缓冲区
    munmap(buf, mmap_size);
    close(fd);
    close(cgroup_fd);

    return 0;
}
