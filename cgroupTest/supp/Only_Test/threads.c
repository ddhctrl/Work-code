#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>

// 获取线程 TID 的函数
pid_t get_tid() {
    return syscall(SYS_gettid);
}

// 线程函数
void *thread_func(void *arg) {
    printf("Thread %d: TID = %ld\n", *(int *)arg, (long)get_tid());
    sleep(20);  // 模拟线程的工作
    return NULL;
}

int main() {
    int num_threads = 3;
    pthread_t threads[num_threads];
    int thread_args[num_threads];

    // 创建多个线程
    for (int i = 0; i < num_threads; ++i) {
        thread_args[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_args[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // 等待所有线程完成
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
