#define _GNU_SOURCE
#include <sys/epoll.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
    int epfd, pipefd[2], nfds;
    struct epoll_event ev, events[2];
    sigset_t sigmask;
    int timeout_ms = 5000;  // 设置延时为 5000 毫秒 (5 秒)

    // 创建一个管道
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // 创建 epoll 实例
    epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // 将标准输入（stdin）添加到 epoll 中
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl: stdin");
        exit(EXIT_FAILURE);
    }

    // 将管道读端添加到 epoll 中
    ev.events = EPOLLIN;
    ev.data.fd = pipefd[0];
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev) == -1) {
        perror("epoll_ctl: pipe");
        exit(EXIT_FAILURE);
    }

    // 设置信号屏蔽集，屏蔽 SIGINT
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);

    printf("Waiting for input on stdin or pipe, SIGINT is temporarily blocked...\n");
    printf("The program will automatically exit after %d milliseconds if no event occurs.\n", timeout_ms);

    // 等待事件，超时时间为 5 秒
    nfds = epoll_pwait(epfd, events, 2, timeout_ms, &sigmask);
    
    if (nfds == -1) {
        perror("epoll_pwait");
        exit(EXIT_FAILURE);
    } else if (nfds == 0) {
        printf("Timeout occurred, no events received. Exiting...\n");
        // 超时没有事件发生，程序退出
    } else {
        // 处理事件
        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == STDIN_FILENO) {
                printf("Data is available on stdin\n");
            } else if (events[n].data.fd == pipefd[0]) {
                printf("Data is available on the pipe\n");
            }
        }
    }

    // 清理资源
    close(epfd);
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
