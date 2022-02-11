#pragma once

#include <atomic>

#include "src/base/fd_guard.h"

class server {
public:
    server();
    ~server();

    int start(std::string ip, uint16_t port, int backlog, int event_num);

private:
    void stop();

private:
    void handler_accept();
    void handler_read(int fd);
    void handler_write(int fd, char* data, size_t size);

    int connect_timeout(int fd, const struct sockaddr_in addr, long nsec, long usec);

private:
    std::atomic<bool> terminate_;
    fd_guard epoll_fd_;
    fd_guard listen_fd_;
};