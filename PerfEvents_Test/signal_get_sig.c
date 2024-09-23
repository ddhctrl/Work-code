#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

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

void sigusr1_handler(int sig) {
    printf("Child process received SIGUSR1\n");
}

void wait_for_signal(pid_t pid) {
    int status;
    while (1) {
        if (waitpid(pid, &status, 0) == -1) {
            handle_error("waitpid");
        }
        if (WIFSTOPPED(status)) {
            printf("Child process stopped by signal %d\n", WSTOPSIG(status));
            break;
        }
    }
}

int main() {
    // 设置SIGINT的处理程序
    signal(SIGINT, sigint_handler);

    child_pid = fork();

    if (child_pid == 0) {
        // 子进程
        struct sigaction sa;
        sa.sa_handler = sigusr1_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGUSR1, &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        execl("/bin/sleep", "sleep", "20", NULL); // 执行一个shell
    } else {
        // 父进程（调试器）
        sleep(1); // 等待子进程启动

        // 使用 PTRACE_SEIZE 模式附加到子进程
        if (ptrace(PTRACE_SEIZE, child_pid, NULL, 0) == -1) {
            handle_error("ptrace PTRACE_SEIZE");
        }

        // 发送 SIGUSR1 信号，使子进程继续执行
        printf("Sending SIGUSR1 to child process...\n");
        if (kill(child_pid, SIGUSR1) == -1) {
            handle_error("kill SIGUSR1");
        }

        // 等待子进程继续运行
        if (waitpid(child_pid, NULL, 0) == -1) {
            handle_error("waitpid");
        }

        // 父进程阻塞，保持子进程继续执行
        printf("Child process is now running.\n");
    }

    return 0;
}
