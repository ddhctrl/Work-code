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

void clear_ptrace_options(pid_t pid) {
    if (ptrace(PTRACE_SETOPTIONS, pid, NULL, 0) == -1) {
        handle_error("ptrace PTRACE_SETOPTIONS");
    }
    printf("Cleared ptrace options for process %d\n", pid);
}

int main() {
    // 设置SIGINT的处理程序
    signal(SIGINT, sigint_handler);

    child_pid = fork();

    if (child_pid == 0) {
        // 子进程
        execl("/bin/sleep", "sleep", "20", NULL); // 执行一个shell
    } else {
        // 父进程（调试器）
        sleep(1); // 等待子进程启动

        // 使用 PTRACE_SEIZE 模式附加到子进程
        if (ptrace(PTRACE_SEIZE, child_pid, NULL, 0) == -1) {
            handle_error("ptrace PTRACE_SEIZE");
        }

        // 暂停子进程
        if (kill(child_pid, SIGSTOP) == -1) {
            handle_error("kill SIGSTOP");
        }

        // 等待子进程被 PTRACE_INTERRUPT 暂停
        wait_for_signal(child_pid);
        printf("Child process stopped by PTRACE_INTERRUPT\n");

        //使用 PTRACE_SETOPTIONS 设置选项以便子进程生成 JOBCTL_TRAP_MASK
        // if (ptrace(PTRACE_SETOPTIONS, child_pid, NULL, PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACESECCOMP) == -1) {
        //     handle_error("ptrace PTRACE_SETOPTIONS");
        // }

        // 发送 SIGCONT 信号，使子进程继续执行
        printf("Sending SIGCONT to child process...\n");
        if (kill(child_pid, SIGCONT) == -1) {
            handle_error("kill SIGCONT");
        }

        // 等待子进程继续运行
        if (waitpid(child_pid, NULL, 0) == -1) {
            handle_error("waitpid");
        }

        // 父进程阻塞，保持子进程继续执行
        printf("Child process is now running. Parent is now blocking...\n");
        pause(); // 阻塞父进程

        // 清除所有 ptrace 选项
        //clear_ptrace_options(child_pid);

        // 终止子进程
        if (kill(child_pid, SIGKILL) == -1) {
            handle_error("kill SIGKILL");
        }

        // 等待子进程退出
        if (waitpid(child_pid, NULL, 0) == -1) {
            handle_error("waitpid");
        }

        printf("Child process terminated\n");
    }

    return 0;
}
