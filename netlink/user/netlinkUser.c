//Taken from https://stackoverflow.com/questions/15215865/netlink-sockets-in-c-using-the-3-x-linux-kernel?lq=1
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#define MAX_PAYLOAD 1024
#define NETLINK_UNICAST_SEND 0
#define DST_KERNEL 0

#define NETLINK_USER 31

int main()
{
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    int sock_fd;
    struct msghdr msg;

    if ((sock_fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_USER)) < 0)
    {
        err(EXIT_FAILURE, "socket() ");
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0)
    {
        err(EXIT_FAILURE, "bind() ");
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = DST_KERNEL;
    dest_addr.nl_groups = 0;

    if ((nlh = (struct nlmsghdr *)calloc(1, NLMSG_SPACE(MAX_PAYLOAD))) == NULL)
    {
        err(EXIT_FAILURE, "calloc() ");
    }

    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), "Hello from userspace");
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset((void *)&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    printf("Sending message to kernel\n");

    if (sendmsg(sock_fd, &msg, 0) < 0)
    {
        err(EXIT_FAILURE, "Sending error:%s", strerror(errno));
    }

    printf("Waiting for message from kernel\n");

    if (recvmsg(sock_fd, &msg, 0) < 0)
    {
        err(EXIT_FAILURE, "Receive error:%s", strerror(errno));
    }

    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
    close(sock_fd);
    return 0;
}
