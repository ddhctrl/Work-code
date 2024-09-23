#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

void* thread_func(void* arg) {
    // 线程函数，执行一些简单的任务
    while (1) {
        sleep(1);
    }
    return NULL;
}

void signal_handler(int sig) {
    // 信号处理程序，不做任何处理
    printf("Signal %d received\n", sig);
}

int main() {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        // 子进程
        // struct sigaction sa;
        // sa.sa_handler = signal_handler;
        // sigemptyset(&sa.sa_mask);
        // sa.sa_flags = 0;
        // if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        //     perror("sigaction");
        //     exit(EXIT_FAILURE);
        // }

        // // 阻塞 SIGUSR1 信号
        // sigset_t sigset;
        // sigemptyset(&sigset);
        // sigaddset(&sigset, SIGUSR1);
        // if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) {
        //     perror("sigprocmask");
        //     exit(EXIT_FAILURE);
        // }

        // // 发送一个 SIGUSR1 信号给自己，使其保持挂起状态
        // if (kill(getpid(), SIGUSR1) == -1) {
        //     perror("kill");
        //     exit(EXIT_FAILURE);
        // }
    
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        // 创建一个新线程
        printf("Create new thread\n");
        pthread_t thread;
        if (pthread_create(&thread, NULL, thread_func, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        // 发送一个 SIGUSR1 信号给自己，使其保持挂起状态
        // if (kill(getpid(), SIGUSR1) == -1) {
        //     perror("kill");
        //     exit(EXIT_FAILURE);
        // }

        // 执行一个命令
        execl("/bin/ls", "ls", NULL); // 这里可以替换为任何其他命令
        perror("execl"); // 如果 execl 返回，说明出错
        exit(EXIT_FAILURE);
    } else {
        // 父进程（调试器）
        int status;

        // 模拟一些处理
        sleep(2);

        // if (ptrace(PTRACE_KILL, child_pid, NULL, NULL) == -1) {
        //     perror("ptrace PTRACE_KILL");
        // }

        // 等待子进程退出
        if (waitpid(child_pid, NULL, 0) == -1) {
            perror("waitpid");
        }
        printf("Child process terminated\n");
    }

    return 0;
}
