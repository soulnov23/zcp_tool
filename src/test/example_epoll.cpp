#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "src/base/log.h"
#include "src/base/net_util.h"
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
        CONSOLE_ERROR("make_socket_nonblocking({}) error", listen_fd);
        return -1;
    }
    struct epoll_event event;
    event.data.fd = listen_fd;
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event)) {
        CONSOLE_ERROR("epoll_ctl({}, EPOLL_CTL_ADD, {}) error", epoll_fd, listen_fd);
        return -1;
    }
    if (-1 == make_socket_reuseaddr(listen_fd)) {
        CONSOLE_ERROR("make_socket_reuseaddr({}) error", listen_fd);
        return -1;
    }
    if (-1 == bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        CONSOLE_ERROR("bind({}, {}) error", listen_fd, inet_ntoa(server_addr.sin_addr));
        close(listen_fd);
        listen_fd = -1;
        return -1;
    }
    if (-1 == listen(listen_fd, 128)) {
        CONSOLE_ERROR("listen({}, 128) error", listen_fd);
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
        CONSOLE_DEBUG("(TCP)New accept ip:{} socket:{}", inet_ntoa(addr.sin_addr), fd);
        if (-1 == make_socket_nonblocking(fd)) {
            CONSOLE_ERROR("make_socket_nonblocking({}) error", fd);
        }
        if (-1 == make_socket_tcpnodelay(fd)) {
            CONSOLE_ERROR("make_socket_tcpnodelay({}) error", fd);
        }
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLOUT | EPOLLET;
        if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
            CONSOLE_ERROR("epoll_ctl({}, EPOLL_CTL_ADD, {}) error", epoll_fd, fd);
        }
    }
}

void do_recv(int fd) {
    while (true) {
        char buf[1024] = {0};
        ssize_t ret = recv(fd, buf, 1024, 0);
        if (ret > 0) {
            CONSOLE_DEBUG("fd:{} recv:{}", fd, buf);
            continue;
        } else if (ret == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                CONSOLE_ERROR("fd:{} abnormal disconnection", fd);
                if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    CONSOLE_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", epoll_fd, fd);
                }
                close(fd);
                break;
            }
        }
        if (ret == 0) {
            CONSOLE_ERROR("fd:{} normal disconnection", fd);
            if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                CONSOLE_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", epoll_fd, fd);
            }
            close(fd);
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    CONSOLE_DEBUG("example_epoll start success");
    CONSOLE_DEBUG("push");
    // cout << "push" << endl;
    epoll_fd = epoll_create1(0);
    if (-1 == epoll_fd) {
        CONSOLE_ERROR("epoll_create1 error");
        return -1;
    }
    if (-1 == do_listen()) {
        CONSOLE_ERROR("do_listen error");
        return -1;
    }
    struct epoll_event events[128];
    while (true) {
        int count = epoll_wait(epoll_fd, events, 128, -1);
        if (-1 == count) {
            CONSOLE_ERROR("epoll_wait({}) error", epoll_fd);
            return -1;
        }
        for (int i = 0; i < count; i++) {
            // CONSOLE_DEBUG("fd:{} event:{}", &(events[i].data.fd), &(events[i].events));
            if (events[i].events & EPOLLIN) {
                if (events[i].data.fd == listen_fd) {
                    do_accept();
                } else {
                    do_recv(events[i].data.fd);
                }
            } else if (events[i].events & EPOLLOUT) {
                CONSOLE_DEBUG("EPOLLOUT");
            }
        }
    }
    return 0;
}
