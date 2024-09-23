#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h> /* For struct user_regs_struct */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        // 子进程
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        raise(SIGSTOP); // 发送 SIGSTOP 信号以停止自己
        execl("/bin/ls", "ls", NULL); // 这里可以替换为任何其他命令
    } else {
        // 父进程（调试器）
        int status;

        // 等待子进程停止
        waitpid(child_pid, &status, WUNTRACED);
        if (WIFSTOPPED(status)) {
            printf("Child process stopped by SIGSTOP\n");
        }

        // 使用 ptrace 继续子进程的执行，并确保进入停止状态
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        kill(child_pid, SIGSTOP); // 发送 SIGSTOP 信号以再次停止子进程
        waitpid(child_pid, &status, WUNTRACED);
        if (WIFSTOPPED(status)) {
            printf("Child process stopped again by SIGSTOP\n");
        }

        // 确保子进程在停止状态
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        waitpid(child_pid, &status, WUNTRACED);
        if (WIFSTOPPED(status)) {
            printf("Child process stopped by SIGSTOP during PTRACE_CONT\n");
        }

        // 模拟一些处理
        sleep(2);

        // 启动子进程
        printf("Resuming child process...\n");
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
