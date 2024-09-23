#include <linux/perf_event.h> /* Definition of PERF_* constants */
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <inttypes.h>

// The function to counting through (called in main)
void code_to_measure(){
    int sum = 0;
    for(int i = 0; i < 1000000000; ++i){
        sum += 1;
    }
}

// Executes perf_event_open syscall and makes sure it is successful or exit
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags){
    return syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct perf_event_attr pe_group_leader, pe_sibling1, pe_sibling2;
    int fd_group_leader, fd_sibling1, fd_sibling2;
    long long count_group_leader, count_sibling1, count_sibling2;

    // Configure the group leader event
    memset(&pe_group_leader, 0, sizeof(struct perf_event_attr));
    pe_group_leader.type = PERF_TYPE_HARDWARE;
    pe_group_leader.size = sizeof(struct perf_event_attr);
    pe_group_leader.config = PERF_COUNT_HW_CPU_CYCLES;
    pe_group_leader.disabled = 1;
    pe_group_leader.exclude_kernel = 1;
    pe_group_leader.exclude_hv = 1;

    // Open the group leader event
    fd_group_leader = perf_event_open(&pe_group_leader, 0, -1, -1, 0);
    if (fd_group_leader == -1) {
        fprintf(stderr, "Error opening group leader event: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Configure the first sibling event
    memset(&pe_sibling1, 0, sizeof(struct perf_event_attr));
    pe_sibling1.type = PERF_TYPE_HARDWARE;
    pe_sibling1.size = sizeof(struct perf_event_attr);
    pe_sibling1.config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
    pe_sibling1.disabled = 1;
    pe_sibling1.exclude_kernel = 1;
    pe_sibling1.exclude_hv = 1;

    // Open the first sibling event, using the group leader fd
    fd_sibling1 = perf_event_open(&pe_sibling1, 0, -1, fd_group_leader, 0);
    if (fd_sibling1 == -1) {
        fprintf(stderr, "Error opening sibling event 1: %s\n", strerror(errno));
        close(fd_group_leader);
        exit(EXIT_FAILURE);
    }

    // Configure the second sibling event
    memset(&pe_sibling2, 0, sizeof(struct perf_event_attr));
    pe_sibling2.type = PERF_TYPE_HARDWARE;
    pe_sibling2.size = sizeof(struct perf_event_attr);
    pe_sibling2.config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
    pe_sibling2.disabled = 1;
    pe_sibling2.exclude_kernel = 1;
    pe_sibling2.exclude_hv = 1;

    // Open the second sibling event, using the group leader fd
    fd_sibling2 = perf_event_open(&pe_sibling2, 0, -1, fd_group_leader, 0);
    if (fd_sibling2 == -1) {
        fprintf(stderr, "Error opening sibling event 2: %s\n", strerror(errno));
        close(fd_group_leader);
        close(fd_sibling1);
        exit(EXIT_FAILURE);
    }

    // Enable the group leader and all sibling events
    ioctl(fd_group_leader, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_group_leader, PERF_EVENT_IOC_ENABLE, 0);

    // Example code to count through
    code_to_measure();

    // Disable the group leader and all sibling events
    ioctl(fd_group_leader, PERF_EVENT_IOC_DISABLE, 0);

    // Read counter values
    read(fd_group_leader, &count_group_leader, sizeof(long long));
    read(fd_sibling1, &count_sibling1, sizeof(long long));
    read(fd_sibling2, &count_sibling2, sizeof(long long));

    // Print results
    printf("CPU cycles: %lld\n", count_group_leader);
    printf("Instructions: %lld\n", count_sibling1);
    printf("Branch misses: %lld\n", count_sibling2);

    // Close file descriptors
    close(fd_group_leader);
    close(fd_sibling1);
    close(fd_sibling2);

    return 0;
}
