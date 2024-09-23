#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>

void handle_signal(int sig) {
    printf("Caught signal %d\n", sig);
}

void* thread_function(void* arg) {
    sigset_t new_mask, old_mask;
    
    printf("Child thread started.\n");

    // 初始化信号集并阻塞 SIGTERM 信号
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGTERM);
    if (pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask) != 0) {
        perror("pthread_sigmask");
        return NULL;
    }

    sleep(10); // 模拟子线程的长时间任务

    // 恢复原来的信号屏蔽字
    if (pthread_sigmask(SIG_SETMASK, &old_mask, NULL) != 0) {
        perror("pthread_sigmask");
        return NULL;
    }

    printf("Child thread finished.\n");
    return NULL;
}

int main() {
    sigset_t new_mask, old_mask, pending;
    pthread_t thread_id;

    // 设置进程组 ID
    setpgid(0, 0);

    // 创建一个子线程
    if (pthread_create(&thread_id, NULL, thread_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    // 设置 SIGINT 和 SIGTERM 的处理程序
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // 初始化信号集并阻塞 SIGTERM 信号
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) == -1) {
        perror("sigprocmask");
        return 1;
    }

    // 确保子线程已经开始并阻塞了信号
    sleep(1);

    // 发送 SIGTERM 信号到进程组
    kill(0, SIGTERM);
    syscall(__NR_tkill, getpid(), SIGTERM);

    // 获取未决信号集
    if (sigpending(&pending) == -1) {
        perror("sigpending");
        return 1;
    }

    // 检查 SIGTERM 是否在未决信号集中
    if (sigismember(&pending, SIGTERM)) {
        printf("SIGTERM is pending\n");
    } else {
        printf("SIGTERM is not pending\n");
    }

    sleep(5);

    // 恢复原来的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }

    // 获取未决信号集
    if (sigpending(&pending) == -1) {
        perror("sigpending");
        return 1;
    }

    // 检查 SIGTERM 是否在未决信号集中
    if (sigismember(&pending, SIGTERM)) {
        printf("SIGTERM is pending\n");
    } else {
        printf("SIGTERM is not pending\n");
    }

    // 等待子线程结束
    if (pthread_join(thread_id, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    return 0;
}
