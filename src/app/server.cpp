#include "server.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "src/base/io_util.h"
#include "src/base/log.h"
#include "src/base/net_util.h"

#define SIZE_1K          (1024)
#define SIZE_1M          (SIZE_1K) * (SIZE_1K)
#define EPOLL_EVENT_SIZE 10240

server::server() : epoll_fd_(-1), listen_fd_(-1) {}

server::~server() {
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    if (listen_fd_ != -1) {
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, listen_fd_, nullptr) == -1) {
            LOG_SYSTEM_ERROR("epoll_ctl del fd: {}", int(listen_fd_));
        }
        close(listen_fd_);
        listen_fd_ = -1;
    }
}

int server::start(std::string ip, uint16_t port, int backlog, int event_num) {
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ == -1) {
        LOG_SYSTEM_ERROR("epoll_create1");
        return -1;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);
    listen_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_fd_ == -1) {
        LOG_SYSTEM_ERROR("socket");
        return -1;
    }
    if (make_socket_nonblocking(listen_fd_) == -1) {
        LOG_ERROR("make_socket_nonblocking fd: {} error", int(listen_fd_));
        return -1;
    }
    if (make_socket_cloexec(listen_fd_) == -1) {
        LOG_ERROR("make_socket_cloexec fd: {} error", int(listen_fd_));
        return -1;
    }
    if (make_socket_reuseaddr(listen_fd_) == -1) {
        LOG_ERROR("make_socket_reuseaddr fd: {} error", int(listen_fd_));
        return -1;
    }
    if (make_socket_reuseport(listen_fd_) == -1) {
        LOG_ERROR("make_socket_reuseport fd: {} error", int(listen_fd_));
        return -1;
    }
    struct epoll_event event;
    event.data.fd = listen_fd_;
    event.events = EPOLLET | EPOLLIN;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &event) == -1) {
        LOG_SYSTEM_ERROR("epoll_ctl add fd: {}", int(listen_fd_));
        return -1;
    }
    if (::bind(listen_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        LOG_SYSTEM_ERROR("bind addr: {}", inet_ntoa(server_addr.sin_addr));
        return -1;
    }
    if (listen(listen_fd_, backlog) == -1) {
        LOG_SYSTEM_ERROR("listen backlog: {}", backlog);
        return -1;
    }
    struct epoll_event* event_list = new epoll_event[event_num];
    while (true) {
        int count = epoll_wait(epoll_fd_, event_list, event_num, -1);
        if (count == -1) {
            LOG_SYSTEM_ERROR("epoll_wait");
        }
        for (int i = 0; i < count; i++) {
            int fd = event_list[i].data.fd;
            if ((event_list[i].events & EPOLLRDHUP) || (event_list[i].events & EPOLLHUP)) {
                // shutdown读关闭
                // close关闭
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                if (getpeername(fd, (struct sockaddr*)(&addr), &len) == -1) {
                    LOG_SYSTEM_ERROR("getpeername fd: {}", fd);
                    continue;
                }
                LOG_DEBUG("fd:{} ip:{} normal disconnection", fd, inet_ntoa(addr.sin_addr));
                if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
                    LOG_SYSTEM_ERROR("epoll_ctl del fd: {}", fd);
                }
            } else if (event_list[i].events & EPOLLERR) {
                // 异常关闭
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                if (getpeername(fd, (struct sockaddr*)(&addr), &len) == -1) {
                    LOG_SYSTEM_ERROR("getpeername fd: {}", fd);
                    continue;
                }
                LOG_DEBUG("fd:{} ip:{} abnormal disconnection", fd, inet_ntoa(addr.sin_addr));
                if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
                    LOG_SYSTEM_ERROR("epoll_ctl del fd: {}", fd);
                }
            } else if (event_list[i].events & EPOLLIN) {
                if (fd == listen_fd_) {
                    handler_accept();
                } else {
                    handler_read(fd);
                }
            } else if (event_list[i].events & EPOLLOUT) {
                handler_write(fd, "hello world", 11);
            }
        }
    }
    delete[] event_list;
    return 0;
}

int server::connect_timeout(int fd, const struct sockaddr_in addr, long nsec, long usec) {
    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == 0) {
        LOG_DEBUG("connect直接成功");
        return 0;
    }
    // ret=-1的情况
    if (errno != EINPROGRESS) {
        LOG_ERROR("connect error");
        return -1;
    }
    struct timeval timeout;
    timeout.tv_sec = nsec;
    timeout.tv_usec = usec;
    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(fd, &write_set);
    int count = select(fd + 1, nullptr, &write_set, nullptr, &timeout);
    if (count == 0) {
        LOG_ERROR("select timeout");
        return -1;
    }
    if (count == -1) {
        LOG_ERROR("select error");
        return -1;
    }
    // count>0的情况
    if (FD_ISSET(fd, &write_set)) {
        int err = 0;
        socklen_t len = sizeof(err);
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1) {
            LOG_ERROR("getsockopt error");
            return -1;
        }
        if (err != 0) {
            LOG_ERROR("connect error");
            return -1;
        }
        return 0;
    } else {
        LOG_ERROR("FD_ISSET error");
        return -1;
    }
}

void server::handler_accept() {
    while (true) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int fd = accept(listen_fd_, (struct sockaddr*)&addr, &addr_len);
        if (fd == -1) {
            if (errno == EAGAIN) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                LOG_SYSTEM_ERROR("accept");
                break;
            }
        }
        LOG_DEBUG("(TCP)new accept ip: {} socket: {} pid: {}", inet_ntoa(addr.sin_addr), fd, getpid());
        if (make_socket_nonblocking(fd) == -1) {
            LOG_ERROR("make_socket_nonblocking fd: {} error", fd);
            continue;
        }
        if (make_socket_cloexec(fd) == -1) {
            LOG_ERROR("make_socket_cloexec fd: {} error", fd);
            continue;
        }
        if (make_socket_tcpnodelay(fd) == -1) {
            LOG_ERROR("make_socket_tcpnodelay fd: {} error", fd);
            continue;
        }
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
        if (-1 == epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event)) {
            LOG_SYSTEM_ERROR("epoll_ctl add fd: {}", fd);
            continue;
        }
        // TODO保存连接
    }
}

void server::handler_read(int fd) {
    char data[SIZE_1K] = {0};
    ssize_t ret = wrap(read, fd, data, SIZE_1K);
    if (ret == 0) {
        return;
    }
    if (ret == -1) {
        if (errno != EAGAIN) {
            LOG_SYSTEM_ERROR("read");
            return;
        }
    }
    LOG_DEBUG("read data: {}", data);
}

void server::handler_write(int fd, char* data, size_t size) {
    ssize_t ret = wrap(write, fd, data, size);
    if (ret == -1) {
        if (errno != EAGAIN) {
            // SIGPIPE
            LOG_SYSTEM_ERROR("write");
            return;
        }
    }
    LOG_DEBUG("write data: {}", data);
}