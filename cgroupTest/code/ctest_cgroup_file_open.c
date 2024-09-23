#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define PROCFS_PATH "/proc/cgfd"

int main() {
    int fd;
    FILE *proc_file;

    // 打开 /sys/fs/cgroup 文件
    fd = open("/sys/fs/cgroup", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /sys/fs/cgroup");
        return -1;
    }

    printf("用户空间 File descriptor: %d\n", fd);

    // 打开 procfs 接口文件
    proc_file = fopen(PROCFS_PATH, "w");
    if (!proc_file) {
        perror("Failed to open proc file");
        close(fd);
        return -1;
    }

    // 将文件描述符写入 procfs 接口
    fprintf(proc_file, "%d\n", fd);
    fclose(proc_file);

    // 暂时保持文件打开状态，以便测试
    sleep(3);

    close(fd);

    return 0;
}
