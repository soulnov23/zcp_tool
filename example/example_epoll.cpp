#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include "printf_utils.h"
#include "net/net_utils.h"
#include <string>
#include <iostream>
using namespace std;

int epoll_fd;
int listen_fd;

int do_listen() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8765);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == make_socket_nonblocking(listen_fd)) {
        PRINTF_ERROR("make_socket_nonblocking(%d) error", listen_fd);
        return -1;
    }
    struct epoll_event event;
    event.data.fd = listen_fd;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event)) {
        PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_ADD, %d) error", epoll_fd,
                     listen_fd);
        return -1;
    }
    if (-1 == make_socket_reuseaddr(listen_fd)) {
        PRINTF_ERROR("make_socket_reuseaddr(%d) error", listen_fd);
        return -1;
    }
    if (-1 ==
        bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        PRINTF_ERROR("bind(%d, %s) error", listen_fd,
                     inet_ntoa(server_addr.sin_addr));
        close(listen_fd);
        listen_fd = -1;
        return -1;
    }
    if (-1 == listen(listen_fd, 128)) {
        PRINTF_ERROR("listen(%d, 128) error", listen_fd);
        close(listen_fd);
        listen_fd = -1;
        return -1;
    }
    return 0;
}

void do_accept() {
    while (true) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int fd = accept(listen_fd, (struct sockaddr*)&addr, &addr_len);
        if (fd == -1) {
            break;
        }
        PRINTF_DEBUG("(TCP)New accept ip:%s socket:%d",
                     inet_ntoa(addr.sin_addr), fd);
        if (-1 == make_socket_nonblocking(fd)) {
            PRINTF_ERROR("make_socket_nonblocking(%d) error", fd);
        }
        if (-1 == make_socket_tcpnodelay(fd)) {
            PRINTF_ERROR("make_socket_tcpnodelay(%d) error", fd);
        }
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLOUT | EPOLLET;
        if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
            PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_ADD, %d) error", epoll_fd,
                         fd);
        }
    }
}

void do_recv(int fd) {
    while (true) {
        char buf[1024] = {0};
        ssize_t ret = recv(fd, buf, 1024, 0);
        if (ret > 0) {
            PRINTF_DEBUG("fd:%d recv:%s", fd, buf);
            continue;
        } else if (ret == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                PRINTF_ERROR("fd:%d abnormal disconnection", fd);
                if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error",
                                 epoll_fd, fd);
                }
                close(fd);
                break;
            }
        }
        if (ret == 0) {
            PRINTF_ERROR("fd:%d normal disconnection", fd);
            if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", epoll_fd,
                             fd);
            }
            close(fd);
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    PRINTF_DEBUG("example_epoll start success");
    PRINTF_DEBUG("push");
    // cout << "push" << endl;
    epoll_fd = epoll_create1(0);
    if (-1 == epoll_fd) {
        PRINTF_ERROR("epoll_create1 error");
        return -1;
    }
    if (-1 == do_listen()) {
        PRINTF_ERROR("do_listen error");
        return -1;
    }
    struct epoll_event events[128];
    while (true) {
        int count = epoll_wait(epoll_fd, events, 128, -1);
        if (-1 == count) {
            PRINTF_ERROR("epoll_wait(%d) error", epoll_fd);
            return -1;
        }
        for (int i = 0; i < count; i++) {
            PRINTF_DEBUG("fd:%d event:%d", events[i].data.fd, events[i].events);
            if (events[i].events & EPOLLIN) {
                if (events[i].data.fd == listen_fd) {
                    do_accept();
                } else {
                    do_recv(events[i].data.fd);
                }
            } else if (events[i].events & EPOLLOUT) {
                PRINTF_DEBUG("EPOLLOUT");
            }
        }
    }
    return 0;
}
