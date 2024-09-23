#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

void cpu_intensive_task() {
    // 一个计算密集型的无限循环
    double x = 0;
    while (1) {
        for (int i = 0; i< 1000000; ++i)
        {
            x += 1.0;
            x -= 1.0;    
        }
        usleep(1000000);
    }
}

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        printf("Child process. Child PID = %d, Parent PID = %d\n",  getpid(), getppid());
        cpu_intensive_task();
    } else {
        printf("Parent process. Child PID = %d, Parent PID = %d\n", pid, getpid());
        wait(NULL);
    }

    return 0;
}

