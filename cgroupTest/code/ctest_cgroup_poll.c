#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define POLL_TIMEOUT 2000 // 等待 5 秒

int main() {
    const char *path = "/sys/fs/cgroup/Example/memory.pressure";
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    struct pollfd pfd = {
        .fd = fd,
        .events = POLLPRI | POLLERR,
    };

    int ret = poll(&pfd, 1, POLL_TIMEOUT);
    if (ret == -1) {
        perror("poll");
        close(fd);
        return EXIT_FAILURE;
    }

    if (ret == 0) {
        printf("poll timeout\n");
    } else if (pfd.revents & POLLPRI) {
        char buffer[1024];
        lseek(fd, 0, SEEK_SET);
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        if (n == -1) {
            perror("read");
            close(fd);
            return EXIT_FAILURE;
        }
        buffer[n] = '\0';
        printf("memory.pressure changed:\n%s\n", buffer);
    }

    close(fd);
    return EXIT_SUCCESS;
}
