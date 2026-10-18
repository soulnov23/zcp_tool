#pragma once

#include <sys/epoll.h>

#include <atomic>
#include <cstdint>
#include <vector>

#include "src/base/epoll_event.h"
#include "src/base/fd_operator.h"

// epoll 的封装，对应 netpoll 的 Epoll。
// 除了 epoll fd 本身，还持有一个 eventfd，用来从信号处理函数唤醒阻塞在 wait() 里的循环
class epoll {
public:
    epoll() = default;
    ~epoll();

    epoll(const epoll&) = delete;
    epoll& operator=(const epoll&) = delete;

    // 创建 epoll fd 和 eventfd，并把 eventfd 以只读的方式注册进去。
    // event_num 是一次 epoll_wait 最多返回的事件数
    int create(int event_num);

    // 注册、改注册、注销统一走这里。成功返回0，失败返回-1
    int control(fd_operator* op, epoll_op op_type);

    // 阻塞等待并派发事件。正常退出（被 close() 唤醒）返回0，致命错误返回-1
    int wait();

    // 唤醒 wait()。可以被信号处理函数安全调用：只做一个原子计数和一次 write
    void close();

    [[nodiscard]] int fd() const { return epoll_fd_; }

    // eventfd，给信号处理函数写 8 字节唤醒用。析构后返回 -1
    [[nodiscard]] int wake_fd() const { return wake_fd_; }

private:
    bool handle(int event_num);
    bool handle_wake();

private:
    int epoll_fd_ = -1;
    int wake_fd_ = -1;
    std::atomic<uint32_t> trigger_{0};
    // eventfd 对应的 operator，只在内部用来识别唤醒事件
    fd_operator* wake_operator_ = nullptr;
    std::vector<struct epoll_event> events_;
    // 本轮需要延后处理 hup 的 operator。做成成员避免事件循环里反复分配
    std::vector<fd_operator*> hups_;
};
