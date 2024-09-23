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
#include <signal.h>

// The function to count through (called in main)
void code_to_measure() {
    int sum = 0;
    for (int i = 0; i < 1000000000; ++i) {
        sum += 1;
    }
}

// Signal handler for SIGIO
void handle_sigio(int signo) {
    printf("Received SIGIO signal: Performance event triggered\n");
}

// Wrapper for perf_event_open syscall
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct perf_event_attr pe;
    int fd;
    uint64_t val;

    // Setup the signal handler for SIGIO
    struct sigaction sa;
    sa.sa_handler = handle_sigio;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Configure perf_event_attr structure
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.sample_period = 3000;  // Set sample period
    pe.sample_type = PERF_SAMPLE_IP;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.freq = 1;
    pe.read_format = PERF_FORMAT_GROUP;

    // Open perf event
    fd = perf_event_open(&pe, -1, 0, -1, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening leader %llx: %s\n", pe.config, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("open event fd: %d\n", fd);

    // Set the owner of the file descriptor to the current process
    if (fcntl(fd, F_SETOWN, getpid()) == -1) {
        perror("fcntl(F_SETOWN)");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Enable async notification for the file descriptor
    if (fcntl(fd, F_SETFL, O_ASYNC) == -1) {
        perror("fcntl(F_SETFL, O_ASYNC)");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Reset counters and start counting
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // Example code to count through
    code_to_measure();

    // Stop counting
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    // Read and print result
    read(fd, &val, sizeof(val));
    printf("CPU cycles: %" PRIu64 "\n", val);

    // Close the event
    close(fd);

    return 0;
}
