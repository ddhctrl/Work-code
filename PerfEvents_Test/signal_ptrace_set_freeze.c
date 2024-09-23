#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

pid_t child_pid = -1;

void handle_error(const char *msg) {
    perror(msg);
    if (child_pid > 0) {
        kill(child_pid, SIGKILL);  // 终止子进程
    }
    exit(EXIT_FAILURE);
}

void sigint_handler(int sig) {
    if (child_pid > 0) {
        kill(child_pid, SIGKILL);  // 终止子进程
    }
    exit(0);  // 退出父进程
}

void freeze_process(pid_t pid) {
    char freezer_path[256];
    int fd;

    // 将进程加入到cgroup
    snprintf(freezer_path, sizeof(freezer_path), "/sys/fs/cgroup/my_freezer/cgroup.procs");
    fd = open(freezer_path, O_WRONLY);
    if (fd == -1) {
        handle_error("open cgroup.procs");
    }

    dprintf(fd, "%d\n", pid);
    close(fd);

    // 冻结cgroup
    snprintf(freezer_path, sizeof(freezer_path), "/sys/fs/cgroup/my_freezer/cgroup.freeze");
    fd = open(freezer_path, O_WRONLY);
    if (fd == -1) {
        handle_error("open cgroup.freeze");
    }

    dprintf(fd, "1\n"); // 冻结
    close(fd);
}

void thaw_process(void) {
    char freezer_path[256];
    int fd;

    snprintf(freezer_path, sizeof(freezer_path), "/sys/fs/cgroup/my_freezer/cgroup.freeze");
    fd = open(freezer_path, O_WRONLY);
    if (fd == -1) {
        handle_error("open cgroup.freeze");
    }

    dprintf(fd, "0\n"); // 解冻
    close(fd);
}

int main() {
    // 设置SIGINT的处理程序
    signal(SIGINT, sigint_handler);

    // 创建 freezer cgroup
    system("mkdir -p /sys/fs/cgroup/my_freezer");

    child_pid = fork();

    if (child_pid == 0) {
        // 子进程
        execl("/bin/sleep", "sleep", "5", NULL); // 执行一个shell
    } else {
        // 父进程（调试器）
        sleep(1); // 等待子进程启动

        // 使用 PTRACE_SEIZE 模式附加到子进程
        // if (ptrace(PTRACE_SEIZE, child_pid, NULL, 0) == -1) {
        //     handle_error("ptrace PTRACE_SEIZE");
        // }

        // 暂停子进程
        // if (kill(child_pid, SIGSTOP) == -1) {
        //     handle_error("kill SIGSTOP");
        // }

        // 冻结子进程
        printf("Freezing child process...\n");
        freeze_process(child_pid);

        // 父进程阻塞，保持子进程冻结
        printf("Child process is now frozen. Parent is now blocking...\n");

        // 解冻并清理 cgroup
        thaw_process();
    }

    sleep(5);
    system("rmdir /sys/fs/cgroup/my_freezer");

    return 0;
}
