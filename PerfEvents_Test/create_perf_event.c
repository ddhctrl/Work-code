#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <errno.h>

int main() {
    struct perf_event_attr attr;
    int fd;
    FILE *fd_file;

    // Initialize the perf_event_attr structure
    memset(&attr, 0, sizeof(struct perf_event_attr));

    attr.type = 7; // CoreSight typically uses RAW type
    attr.config = 0xa7c3dddd; // Replace with actual CoreSight event configuration
    attr.size = sizeof(struct perf_event_attr);
    attr.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID;
    attr.sample_period = 1000;
    attr.wakeup_events = 1;
    attr.aux_output = 1; // Enable AUX output

    // attr.type = PERF_TYPE_HARDWARE;
    // attr.config = PERF_COUNT_HW_CPU_CYCLES; // Simple hardware event
    // attr.size = sizeof(struct perf_event_attr);
    // attr.sample_period = 1000;
    // attr.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID;
    // attr.wakeup_events = 1;
    // attr.precise_ip = 1;
    // attr.exclude_kernel = 0;
    // attr.exclude_hv = 0;

    // Use perf_event_open to create the perf event
    fd = syscall(__NR_perf_event_open, &attr, 0, 0, -1, PERF_FLAG_FD_NO_GROUP);
    if (fd == -1) {
        perror("perf_event_open");
        return -1;
    }

    printf("Perf event created with fd: %d\n", fd);

    while (access("/proc/perf_fd", F_OK) == -1) {
        usleep(100000); // Sleep for 100ms
    }

    fd_file = fopen("/proc/perf_fd", "w");
    if (fd_file == NULL) {
        perror("fopen");
        close(fd);
        return -1;
    }

    fprintf(fd_file, "%d", fd);
    fclose(fd_file);

    // Keep the process running to maintain the event
    while (1) {
        sleep(1);
    }

    close(fd);
    return 0;
}
