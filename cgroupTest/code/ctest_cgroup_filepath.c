#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define PROCFS_FPATH "/proc/cgpath"

int main() {
    const char *cgroup_path = "user.slice";
    FILE *proc_file;

    printf("用户空间 File path: %s\n", cgroup_path);

    // 打开 procfs 接口文件
    proc_file = fopen(PROCFS_FPATH, "w");
    if (!proc_file) {
        perror("Failed to open proc file");
        return -1;
    }

    // 将路径字符串写入 procfs 接口
    fprintf(proc_file, "%s\n", cgroup_path);
    fclose(proc_file);

    printf("Path passed to kernel module: %s\n", cgroup_path);

    return 0;
}
