#include "src/app/server.h"

#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstring>

#include "src/base/eintr.h"
#include "src/base/log.h"
#include "src/base/net_util.h"
#include "src/base/socket.h"

namespace {

    // 每条连接的对端地址。accept 时格式化好，打印和断开时用，
    // 那时候 fd 可能已经关了，getpeername 会失败
    struct connection_info {
        std::string peer;
    };

    // 回调是普通函数指针，拿不到 this。进程内同时只有一个 server 在跑
    server* g_server = nullptr;

    connection_info* get_info(fd_operator* op) { return static_cast<connection_info*>(op->data); }

}  // namespace

server::server() { g_server = this; }

server::~server() {
    // epoll 析构时会关掉它的 fd，先把信号处理函数和唤醒 fd 的关联断开，
    // 避免 fd 号被回收后再被写
    if (wake_fd_target_ != nullptr) {
        *wake_fd_target_ = -1;
    }
    epoll_.reset();
    if (listen_fd_ != -1) {
        // close 不能重试：EINTR 时 fd 可能已经关掉了，再调一次会关掉别的 fd
        static_cast<void>(ignore_eintr(::close, listen_fd_));
        listen_fd_ = -1;
    }
    g_server = nullptr;
}

int server::start(std::string ip, uint16_t port, int backlog, int event_num) {
    listen_fd_ = create_socket("tcp");
    if (listen_fd_ == -1) {
        LOG_ERROR("create socket error");
        return -1;
    }
    if (make_socket_reuseaddr(listen_fd_) != 0) {
        LOG_ERROR("make_socket_reuseaddr fd: {} error", listen_fd_);
        return -1;
    }
    // 多个子进程监听同一个端口，靠它让内核把新连接分流
    if (make_socket_reuseport(listen_fd_) != 0) {
        LOG_ERROR("make_socket_reuseport fd: {} error", listen_fd_);
        return -1;
    }

    struct sockaddr_storage addr {};
    socklen_t addr_len = 0;
    const std::string address = ip + ":" + std::to_string(port);
    if (resolve_sockaddr("tcp", address, &addr, &addr_len, true) != 0) {
        LOG_ERROR("resolve address: {} error", address);
        return -1;
    }
    if (::bind(listen_fd_, reinterpret_cast<struct sockaddr*>(&addr), addr_len) == -1) {
        LOG_SYSTEM_ERROR("bind address: {}", address);
        return -1;
    }
    // 配置里超过 somaxconn 的部分会被内核静默截断，这里先截
    const int real_backlog = std::min(backlog, max_listener_backlog());
    if (::listen(listen_fd_, real_backlog) == -1) {
        LOG_SYSTEM_ERROR("listen backlog: {}", real_backlog);
        return -1;
    }

    epoll_ = std::make_unique<epoll>();
    if (epoll_->create(event_num) != 0) {
        LOG_ERROR("create epoll error");
        return -1;
    }
    // eventfd 建好之后才能把 fd 交给信号处理函数
    if (wake_fd_target_ != nullptr) {
        *wake_fd_target_ = epoll_->wake_fd();
    }

    // listen fd 只关注读事件。挂 EPOLLOUT 会让它每轮都就绪（监听 socket 的
    // 发送缓冲区永远是空的），水平触发下会满核忙转
    listen_operator_ = new fd_operator(listen_fd_, epoll_.get());
    listen_operator_->on_read = accept_handler;
    listen_operator_->on_hup = hup_handler;
    if (epoll_->control(listen_operator_, epoll_op::readable) != 0) {
        LOG_ERROR("control listen_fd: {} error", listen_fd_);
        delete listen_operator_;
        listen_operator_ = nullptr;
        return -1;
    }
    LOG_INFO("listen success ip: {} port: {} backlog: {} epoll_fd: {} pid: {}", ip, port, real_backlog, epoll_->fd(),
             getpid());

    const int ret = epoll_->wait();
    if (ret != 0) {
        LOG_ERROR("epoll wait error");
        return -1;
    }
    LOG_INFO("server exit pid: {}", getpid());
    return 0;
}

void server::stop() {
    if (epoll_) {
        epoll_->close();
    }
}

int server::wake_fd() const { return epoll_ != nullptr ? epoll_->wake_fd() : -1; }

void server::accept_handler(epoll* ep, fd_operator* op) {
    if (g_server != nullptr) {
        g_server->handler_accept(ep, op);
    }
}

void server::read_handler(epoll* ep, fd_operator* op) {
    if (g_server != nullptr) {
        g_server->handler_read(ep, op);
    }
}

void server::hup_handler(epoll* ep, fd_operator* op) {
    if (g_server != nullptr) {
        g_server->handler_hup(ep, op);
    }
}

void server::handler_accept(epoll* ep, fd_operator* op) {
    // 水平触发下一次取一个也正确，被唤醒一次就把队列取空更划算
    while (true) {
        struct sockaddr_storage addr {};
        socklen_t addr_len = 0;
        const int fd = accept_socket(op->fd, &addr, &addr_len);
        if (fd == -1) {
            if (errno == EAGAIN) {
                return;
            }
            if (errno == EINTR) {
                continue;
            }
            // EMFILE/ENFILE 是 fd 耗尽，继续 accept 只会刷日志
            LOG_SYSTEM_ERROR("accept listen_fd: {}", op->fd);
            return;
        }
        if (make_socket_tcpnodelay(fd) != 0) {
            LOG_ERROR("make_socket_tcpnodelay fd: {} error", fd);
            static_cast<void>(ignore_eintr(::close, fd));
            continue;
        }

        auto* info = new connection_info();
        info->peer = sockaddr_to_string(reinterpret_cast<struct sockaddr*>(&addr), addr_len);

        fd_operator* client = new fd_operator(fd, ep);
        client->data = info;
        client->on_read = read_handler;
        client->on_hup = hup_handler;
        if (ep->control(client, epoll_op::readable) != 0) {
            LOG_ERROR("control client_fd: {} error", fd);
            delete info;
            delete client;
            static_cast<void>(ignore_eintr(::close, fd));
            continue;
        }
        LOG_INFO("connected fd: {} peer: {} pid: {}", fd, info->peer, getpid());
    }
}

void server::handler_read(epoll* ep, fd_operator* op) {
    static_cast<void>(ep);
    auto* info = get_info(op);
    if (info == nullptr) {
        LOG_ERROR("fd: {} has no connection info", op->fd);
        return;
    }
    // 循环读到没数据为止，单轮上限 8K。EINTR 由 handle_eintr 内部重试，
    // 不会出现在这里的 errno 分支里
    char buffer[read_once_size];
    size_t offset = 0;
    while (offset < sizeof(buffer)) {
        const ssize_t ret = handle_eintr([&] { return ::read(op->fd, buffer + offset, sizeof(buffer) - offset); });
        if (ret == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 非阻塞 IO 没有数据可读，等下一次事件。这条路径最常走，不打日志
                break;
            }
            // EBADF/EINVAL 在"hup 放到读之后处理"的模型里不会出现；
            // ECONNRESET 是读的过程中对端发了 RST。两类都只打日志，
            // 关闭统一交给 hup，避免在读路径里重复清理
            LOG_SYSTEM_ERROR("read fd: {} peer: {}", op->fd, info->peer);
            break;
        }
        if (ret == 0) {
            // 对端调了 close，TCP 发来 FIN，同样等 hup 收尾
            break;
        }
        offset += static_cast<size_t>(ret);
    }
    if (offset == 0) {
        // netpoll 这里无条件打一条，但 EOF 时紧跟着就是断开日志，重复了，这里跳过
        return;
    }
    // 只打实际读到的长度，buffer 其余部分是未初始化的栈内容
    LOG_INFO("read fd: {} peer: {} size: {} data: {:.{}}", op->fd, info->peer, offset, buffer,
             static_cast<int>(offset));
}

void server::handler_hup(epoll* ep, fd_operator* op) {
    if (op == listen_operator_) {
        LOG_ERROR("listen_fd: {} hup, stop server", listen_fd_);
        stop();
        return;
    }
    static_cast<void>(ep);
    auto* info = get_info(op);
    LOG_INFO("disconnected fd: {} peer: {} pid: {}", op->fd, info != nullptr ? info->peer : "", getpid());
    dispose(op);
}

void server::dispose(fd_operator* op) {
    if (op->fd >= 0) {
        static_cast<void>(ignore_eintr(::close, op->fd));
        op->fd = -1;
    }
    delete get_info(op);
    op->data = nullptr;
    delete op;
}
