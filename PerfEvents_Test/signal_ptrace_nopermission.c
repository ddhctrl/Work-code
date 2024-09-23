#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>

void *thread_func(void *arg) {
    printf("Thread %ld started\n", (long)arg);

    if ((long)arg == 1) {
        // 线程1: 等待接收信号
        pause();
        printf("Thread 1: Signal received\n");
    } else if ((long)arg == 2) {
        // 线程2: 尝试发送信号给线程1
        sleep(2); // 确保线程1已经在等待
        printf("Thread 2: Attempting to send signal to Thread 1\n");

        // 尝试发送信号
        if (pthread_kill((pthread_t)arg, SIGUSR1) != 0) {
            perror("Thread 2: pthread_kill failed");
        } else {
            printf("Thread 2: Signal sent successfully\n");
        }
    }

    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    uid_t new_uid = 65534; // 使用一个普通用户ID来模拟权限不足

    printf("Main thread: Creating threads\n");

    // 创建线程1
    if (pthread_create(&tid1, NULL, thread_func, (void *)1L) != 0) {
        perror("pthread_create for thread 1");
        exit(EXIT_FAILURE);
    }

    // 创建线程2
    if (pthread_create(&tid2, NULL, thread_func, (void *)2L) != 0) {
        perror("pthread_create for thread 2");
        exit(EXIT_FAILURE);
    }

    // 更改线程2的UID来模拟权限不足
    if (seteuid(new_uid) != 0) {
        perror("seteuid");
        exit(EXIT_FAILURE);
    }

    // 等待线程结束
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("Main thread: All threads completed\n");

    return 0;
}
