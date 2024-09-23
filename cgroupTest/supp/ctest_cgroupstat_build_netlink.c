#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/taskstats.h>
#include <linux/genetlink.h>
#include <linux/cgroupstats.h>
#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//#define CGROUPSTATS_CMD_GET 4

int main() {
    struct nl_sock *sock;
    int family_id;
    struct nl_msg *msg;
    int ret;
    int fd;

    fd = open("/proc/self/cgroup", O_RDONLY);
    if (fd < 0) {
        perror("Unable to open /proc/self/cgroup");
        return -1;
    }

    // 创建一个 Netlink 套接字
    sock = nl_socket_alloc();
    if (!sock) {
        perror("Unable to allocate socket");
        return -1;
    }

    // 连接到 Generic Netlink 控制接口
    if (genl_connect(sock)) {
        perror("Unable to connect to generic netlink");
        nl_socket_free(sock);
        return -1;
    }

    // 查找 cgroupstats 家族 ID
    family_id = genl_ctrl_resolve(sock, TASKSTATS_GENL_NAME);
    if (family_id < 0) {
        fprintf(stderr, "Unable to resolve taskstats family id\n");
        nl_socket_free(sock);
        return -1;
    }

    // 构造 Netlink 消息
    msg = nlmsg_alloc();
    if (!msg) {
        perror("Unable to allocate message");
        nl_socket_free(sock);
        return -1;
    }

    genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0, CGROUPSTATS_CMD_GET, TASKSTATS_VERSION);

    ret = nla_put_u32(msg, CGROUPSTATS_CMD_ATTR_FD, fd);
    if (ret < 0) {
        fprintf(stderr, "Unable to add attribute: %d\n", ret);
        nlmsg_free(msg);
        nl_socket_free(sock);
        return -1;
    }


    fprintf(stdout, "netlink send message\n");
    // 发送消息
    ret = nl_send_auto(sock, msg);
    if (ret < 0) {
        fprintf(stderr, "Unable to send message: %d\n", ret);
        nlmsg_free(msg);
        nl_socket_free(sock);
        return -1;
    }

    // 处理回复
    // 这里省略了接收和处理回复的代码，可以根据具体需求添加

    nlmsg_free(msg);
    nl_socket_free(sock);
    close(fd);
    return 0;
}
