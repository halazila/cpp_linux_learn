#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>

struct Channel
{
    int wpid; //worker pid
    int fd;
    Channel()
    {
        wpid = fd = -1;
    }
};

int send_fd(int sock, int fd);
int recv_fd(int sock);

#define WORKER_CNT 2
#define MAX_EVENTS 16
#define MAX_BUFSIZE 128

int main(int argc, char *argv[])
{
    Channel workChnel[WORKER_CNT];
    int sockPair[2];
    for (int i = 0; i < WORKER_CNT; i++)
    {
        socketpair(AF_UNIX, SOCK_STREAM, 0, sockPair);
        int _pid = fork();
        if (_pid < 0)
        {
            std::cout << "fork error: " << strerror(errno) << std::endl;
            break;
        }
        else if (_pid == 0) //child process
        {
            int pfd = sockPair[1]; //current process socket
            int index = i;
            close(sockPair[0]);
            while (++i < WORKER_CNT) //recv brother process socket
            {
                int t = recv_fd(pfd);
                workChnel[i].fd = t;
            }
            //epoll
            int epfd = epoll_create(16);
            if (epfd < 0)
            {
                perror("epoll_create");
                exit(-1);
            }
            epoll_event ev, events[MAX_EVENTS];
            for (int j = 0; j < WORKER_CNT; j++)
            {
                if (j != index && workChnel[j].fd > 0)
                {
                    ev.events = EPOLLOUT;
                    ev.data.fd = workChnel[j].fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, workChnel[j].fd, &ev);
                }
            }
            ev.events = EPOLLIN | EPOLLOUT;
            ev.data.fd = pfd;
            epoll_ctl(epfd, EPOLL_CTL_ADD, pfd, &ev);
            char recvbuf[MAX_BUFSIZE];
            char sendbuf[MAX_BUFSIZE];
            int mark = 0;
            for (; mark < 120; mark++)
            {
                sleep(1);
                int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
                if (nfds < 0)
                {
                    perror("epoll_wait");
                    exit(-1);
                }
                for (int n = 0; n < nfds; n++)
                {
                    if (events[n].events & EPOLLIN)
                    {
                        memset(recvbuf, 0, MAX_BUFSIZE);
                        if (read(events[n].data.fd, recvbuf, MAX_BUFSIZE) < 0)
                        {
                            //
                        }
                        else
                            printf("child process %d, recv data: %s\n", getpid(), recvbuf);
                    }
                    if (events[n].events & EPOLLOUT)
                    {
                        memset(sendbuf, 0, MAX_BUFSIZE);
                        sprintf(sendbuf, "Hello %d from child process %d", mark, getpid());
                        if (write(events[n].data.fd, sendbuf, strlen(sendbuf)) < 0)
                        {
                            //
                        }
                    }
                }
            }
            exit(0);
        }
        else //parent process
        {
            workChnel[i].wpid = _pid;
            workChnel[i].fd = sockPair[0];
            close(sockPair[1]);
            //send fd to child process created before
            for (int j = 0; j < i; j++)
            {
                send_fd(workChnel[j].fd, workChnel[i].fd);
            }
        }
    }
    //epoll
    int epfd = epoll_create(16);
    if (epfd < 0)
    {
        perror("epoll_create");
        exit(-1);
    }
    epoll_event ev, events[MAX_EVENTS];
    for (int i = 0; i < WORKER_CNT; i++)
    {
        ev.data.fd = workChnel[i].fd;
        ev.events = EPOLLOUT | EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, workChnel[i].fd, &ev);
    }
    char recvbuf[MAX_BUFSIZE];
    char sendbuf[MAX_BUFSIZE];
    for (;;)
    {
        sleep(1);
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds < 0)
        {
            perror("epoll_wait");
            exit(-1);
        }
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                memset(recvbuf, 0, sizeof(recvbuf));
                if (read(events[i].data.fd, recvbuf, sizeof(recvbuf)) < 0)
                {
                    //
                }
                else
                    printf("parent process %d, recv data: %s\n", getpid(), recvbuf);
            }
            if (events[i].events & EPOLLOUT)
            {
                memset(sendbuf, 0, MAX_BUFSIZE);
                sprintf(sendbuf, "Hello from parent process %d", getpid());
                if (write(events[i].data.fd, sendbuf, strlen(sendbuf)) < 0)
                {
                    //
                }
            }
        }
    }

    return 0;
}

int send_fd(int sock, int fd)
{
    char c = '0';
    iovec iov = {.iov_base = &c, .iov_len = 1};

    char buf[CMSG_LEN(sizeof(fd))];
    memset(buf, 0, sizeof(buf));

    msghdr msg;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
    *((int *)CMSG_DATA(cmsg)) = fd;
    int ret = sendmsg(fd, &msg, 0);
    if (ret < 0)
    {
        std::cout << "sendmsg error: " << strerror(errno) << std::endl;
    }
    return ret;
}
int recv_fd(int sock)
{
    msghdr msg = {0};
    char buf[32];
    iovec iov = {.iov_base = buf, .iov_len = sizeof(buf)};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    char cmsgbuf[CMSG_LEN(sizeof(int))];
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    int ret = recvmsg(sock, &msg, 0);
    if (ret < 0)
    {
        std::cout << "recvmsg error: " << strerror(errno) << std::endl;
        return -1;
    }
    int fd = *((int *)CMSG_DATA(cmsg));
    return fd;
}