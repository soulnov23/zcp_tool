#pragma once

class epoll;

// fd 的回调载体。注册进 epoll 之后 epoll_event.data.ptr 直接指向它，
// 同一个 fd 的事件不需要再靠 fd 号反查。
// 生命周期由调用方掌握：alloc 出来，在 on_hup 里 free
class fd_operator {
public:
    fd_operator(int fd, epoll* owner) : fd(fd), owner(owner) {}

    // 三个回调都可以为空，事件循环里逐次判空
    using callback_t = void (*)(epoll*, fd_operator*);

    callback_t on_read = nullptr;
    callback_t on_write = nullptr;
    callback_t on_hup = nullptr;

    // 连接上下文，所有权归调用方，在 on_hup 里释放
    void* data = nullptr;

    int fd = -1;
    epoll* owner = nullptr;
};
