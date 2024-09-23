#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#include <sys/mount.h>
#include <sys/stat.h>

// 内核模块文件路径
#define MODULE_PATH "./ctest_cgroup_sk_clone.ko"

// 获取内核模块参数字符串
void get_module_param(char *param_buf, size_t buf_len, int sockfd) {
    snprintf(param_buf, buf_len, "sockfd=%d", sockfd);
}

// 加载内核模块
int load_kernel_module(const char *module_path, const char *params) {
    int fd;
    void *image;
    struct stat st;
    int ret;

    fd = open(module_path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open module file");
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        perror("Failed to get file status");
        close(fd);
        return -1;
    }

    image = malloc(st.st_size);
    if (!image) {
        perror("Failed to allocate memory");
        close(fd);
        return -1;
    }

    if (read(fd, image, st.st_size) != st.st_size) {
        perror("Failed to read module file");
        free(image);
        close(fd);
        return -1;
    }

    close(fd);

    ret = syscall(__NR_init_module, image, st.st_size, params);
    free(image);

    if (ret < 0) {
        perror("Failed to load module");
        return -1;
    }

    return 0;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char param_buf[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket is listening on port 12345. sockfd: %d\n", sockfd);

    // 获取内核模块参数字符串
    get_module_param(param_buf, sizeof(param_buf), sockfd);

    //加载内核模块
    if (load_kernel_module(MODULE_PATH, param_buf) < 0) {
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Kernel module loaded with parameters: %s\n", param_buf);

    sleep(2); // 等待用户输入以退出并关闭socket

    // 卸载内核模块
    if (syscall(__NR_delete_module, "ctest_cgroup_sk_clone", O_NONBLOCK) < 0) {
        perror("Failed to unload module");
    }

    close(sockfd);
    return 0;
}
