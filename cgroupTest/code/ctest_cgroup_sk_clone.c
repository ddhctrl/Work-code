#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/preempt.h>

// 用于传递 socket 文件描述符的模块参数
static int sockfd = -1;
module_param(sockfd, int, 0444);
MODULE_PARM_DESC(sockfd, "Socket file descriptor");

static struct socket *get_socket_from_fd(int fd)
{
    struct file *file;
    struct socket *sock = NULL;

    file = fget(fd);
    if (file) {
        if (S_ISSOCK(file->f_inode->i_mode))
            sock = file->private_data;
        fput(file);
    }
    return sock;
}

static int __init test_module_init(void)
{
    struct socket *sock;
    struct sock *sk, *new_sk;

    if (sockfd < 0) {
        pr_err("Invalid socket file descriptor\n");
        return -EINVAL;
    }

    sock = get_socket_from_fd(sockfd);
    if (!sock) {
        pr_err("Failed to get socket from file descriptor\n");
        return -ENOENT;
    }

    sk = sock->sk;
    if (!sk) {
        pr_err("Socket has no sock struct\n");
        return -EINVAL;
    }

    preempt_disable();
    new_sk = sk_clone_lock(sk, GFP_KERNEL);
    if (!new_sk) {
        pr_err("Failed to clone socket\n");
        preempt_enable();
        return -ENOMEM;
    }

    pr_info("Successfully cloned socket\n");
    sock_put(new_sk);

    preempt_enable();

    return 0;
}

static void __exit test_module_exit(void)
{
    pr_info("Unloading test module\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A simple kernel module to call sk_clone_lock");
MODULE_VERSION("1.0");

module_init(test_module_init);
module_exit(test_module_exit);
