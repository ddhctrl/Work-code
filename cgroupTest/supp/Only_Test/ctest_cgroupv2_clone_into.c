#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <linux/sched.h>

// clone3 系统调用封装
int clone3(struct clone_args *args, size_t size) {
    return syscall(SYS_clone3, args, size);
}

int main() {
    const char *cgroup_dir_path = "/sys/fs/cgroup/Example";
    
    // 获取 cgroup 目录的文件描述符
    int cgroup_fd = open(cgroup_dir_path, O_DIRECTORY | O_RDONLY);
    if (cgroup_fd == -1) {
        perror("open cgroup directory");
        exit(EXIT_FAILURE);
    }
    // 设置 clone3 参数
    struct clone_args args = {
        .flags = CLONE_INTO_CGROUP,   // 设置标志位 CLONE_INTO_CGROUP
        .cgroup = cgroup_fd  // 使用文件描述符，而不是路径
    };

    // 创建子进程
    pid_t child_pid = clone3(&args, sizeof(args));

    if (child_pid == -1) {
        perror("clone3");
        close(cgroup_fd);  // 关闭文件描述符
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // 子进程代码：显示子进程信息
        printf("Child process created in cgroup: %s\n", cgroup_dir_path);
        printf("Child PID: %d\n", getpid());
        while (1) {
            sleep(1);
        }
    } else {
        // 父进程代码：显示子进程 PID
        printf("Parent process created child with PID: %d\n", child_pid);
    }

    close(cgroup_fd);  // 关闭文件描述符
    return 0;
}

