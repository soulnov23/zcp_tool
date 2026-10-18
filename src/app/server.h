#pragma once

#include <csignal>
#include <cstdint>
#include <memory>
#include <string>

#include "src/base/epoll.h"
#include "src/base/fd_operator.h"

class server {
public:
    server();
    ~server();

    server(const server&) = delete;
    server& operator=(const server&) = delete;

    // 启动服务并进入事件循环，返回0表示正常退出，-1表示出错。
    // 阻塞在这里直到收到 SIGTERM/SIGQUIT，由信号处理函数往 eventfd 写一个数唤醒
    int start(std::string ip, uint16_t port, int backlog, int event_num);

    // 唤醒 fd。eventfd 在 start() 里才建出来，之前返回 -1
    [[nodiscard]] int wake_fd() const;

    // 把唤醒 fd 的落点交给调用方。信号处理函数不能取 server 的成员（对象可能已析构），
    // 由 app 提供一个文件作用域的 sig_atomic_t 变量承接
    void set_wake_fd_target(volatile sig_atomic_t* target) { wake_fd_target_ = target; }

private:
    void stop();

private:
    // fd_operator 的回调是普通函数指针，拿不到 this。进程内只有一个 server
    static void accept_handler(epoll* ep, fd_operator* op);
    static void read_handler(epoll* ep, fd_operator* op);
    static void hup_handler(epoll* ep, fd_operator* op);

private:
    void handler_accept(epoll* ep, fd_operator* op);
    void handler_read(epoll* ep, fd_operator* op);
    void handler_hup(epoll* ep, fd_operator* op);

    // 关 fd、释放连接上下文、删除 operator。所有断开路径都收敛到这里
    void dispose(fd_operator* op);

private:
    // 一轮最多读这么多就让出，对齐 netpoll 的 buffer.Block8k。
    // 不设上限的话，一条持续有数据的连接会把整轮事件循环占住，其它连接被饿死
    static constexpr size_t read_once_size = 8 * 1024;

    int listen_fd_ = -1;
    std::unique_ptr<epoll> epoll_;
    // 监听 fd 的 operator，单独记着便于和连接区分
    fd_operator* listen_operator_ = nullptr;
    // 唤醒 fd 的落点，见 set_wake_fd_target
    volatile sig_atomic_t* wake_fd_target_ = nullptr;
};
