#include <stdio.h>
#include <stdlib.h>
#include <bpf/libbpf.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <errno.h>
#include <sys/ioctl.h>  // 包含 ioctl 头文件

// 定义 perf_event_open 系统调用
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct bpf_object *obj;
    struct bpf_link *link;
    int perf_fd;
    struct perf_event_attr pe = {};

    // 设置 perf_event 用于监控 KSYMBOL BPF 事件
    pe.type = PERF_TYPE_SOFTWARE;  // 软件事件
    pe.size = sizeof(struct perf_event_attr);
    pe.ksymbol = 1;  // 启用 ksymbol 事件
    pe.bpf_event = 1;  // 启用 BPF 相关事件
    pe.sample_type = PERF_SAMPLE_RAW;  // 采样原始事件数据
    pe.sample_period = 1;  // 每次符号加载都触发
    pe.disabled = 1;  // 事件启动时关闭

    // 打开 perf 事件，绑定到当前进程的第 0 个 CPU
    perf_fd = perf_event_open(&pe, -1, 0, -1, 0);
    if (perf_fd == -1) {
        fprintf(stderr, "Error opening perf event: %s\n", strerror(errno));
        return 1;
    }

    // 加载 BPF 对象文件
    obj = bpf_object__open_file("syscall_bpf_prog.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "Error opening BPF object file\n");
        return 1;
    }

    // 加载 BPF 程序到内核
    if (bpf_object__load(obj)) {
        fprintf(stderr, "Error loading BPF object\n");
        return 1;
    }

    // 查找并附加 BPF 程序到 perf 事件
    link = bpf_program__attach_perf_event(bpf_object__find_program_by_name(obj, "handle_ksymbol_bpf_event"), perf_fd);
    if (!link) {
        fprintf(stderr, "Error attaching BPF program to perf event\n");
        return 1;
    }

    printf("BPF program attached to PERF_RECORD_KSYMBOL_TYPE_BPF event.\n");

    // 启用 perf 事件
    ioctl(perf_fd, PERF_EVENT_IOC_ENABLE, 0);

    sleep(3);

    // 禁用 perf 事件
    ioctl(perf_fd, PERF_EVENT_IOC_DISABLE, 0);

    // 卸载 BPF 程序
    bpf_link__destroy(link);
    bpf_object__close(obj);
    close(perf_fd);

    return 0;
}
