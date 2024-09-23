#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef O_PATH
#define O_PATH 010000000
#endif

int main() {
    struct stat statbuf;
    int fd;

    // 打开 cgroup 目录
    fd = open("/sys/fs/cgroup/user.slice", O_PATH);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 获取文件的 stat 信息
    if (fstat(fd, &statbuf) == -1) {
        perror("fstat");
        return 1;
    }

    // 打印 inode 号（这可以被视为 kn->id 的替代）
    printf("Cgroup ID (inode): %lu\n", statbuf.st_ino);

    close(fd);
    return 0;
}
