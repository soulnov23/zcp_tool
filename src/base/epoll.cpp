#include "src/base/epoll.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdint>

#include "src/base/eintr.h"
#include "src/base/log.h"

// 正常退出时 wait() 已经把两个 fd 关掉并置 -1 了，这里只兜底两种情况：
// create() 半路失败，以及建好之后没进过事件循环就析构
epoll::~epoll() {
    // 先置 -1 再关：信号处理函数看到 -1 就直接返回，
    // 避免 fd 号被后续 open 复用（比如重开日志文件）时被写进 8 字节垃圾
    const int wake_fd = wake_fd_;
    wake_fd_ = -1;
    delete wake_operator_;
    wake_operator_ = nullptr;
    if (epoll_fd_ != -1) {
        static_cast<void>(ignore_eintr(::close, epoll_fd_));
        epoll_fd_ = -1;
    }
    if (wake_fd != -1) {
        static_cast<void>(ignore_eintr(::close, wake_fd));
    }
}

int epoll::create(int event_num) {
    events_.resize(static_cast<std::size_t>(std::max(event_num, 1)));

    epoll_fd_ = handle_eintr(::epoll_create1, EPOLL_CLOEXEC);
    if (epoll_fd_ == -1) {
        LOG_SYSTEM_ERROR("epoll_create1");
        return -1;
    }
    // EFD_NONBLOCK 让后续的读不会阻塞，EFD_CLOEXEC 防止 fd 漏进子进程
    wake_fd_ = handle_eintr(::eventfd, 0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wake_fd_ == -1) {
        LOG_SYSTEM_ERROR("eventfd");
        return -1;
    }
    wake_operator_ = new fd_operator(wake_fd_, this);
    if (control(wake_operator_, epoll_op::readable) == -1) {
        LOG_ERROR("epoll_fd: {} control event_fd: {} error", epoll_fd_, wake_fd_);
        return -1;
    }
    LOG_INFO("new epoll epoll_fd: {} event_fd: {}", epoll_fd_, wake_fd_);
    return 0;
}

int epoll::control(fd_operator* op, epoll_op op_type) {
    if (op == nullptr) {
        LOG_ERROR("epoll_fd: {} control operator is nullptr", epoll_fd_);
        return -1;
    }
    int operation = 0;
    uint32_t events = 0;
    switch (op_type) {
        case epoll_op::readable:
            operation = EPOLL_CTL_ADD;
            events = read_flags;
            break;
        case epoll_op::mod_readable:
            operation = EPOLL_CTL_MOD;
            events = read_flags;
            break;
        case epoll_op::writable:
            operation = EPOLL_CTL_ADD;
            events = write_flags;
            break;
        case epoll_op::mod_writable:
            operation = EPOLL_CTL_MOD;
            events = write_flags;
            break;
        case epoll_op::read_writable:
            operation = EPOLL_CTL_ADD;
            events = read_write_flags;
            break;
        case epoll_op::mod_read_writable:
            operation = EPOLL_CTL_MOD;
            events = read_write_flags;
            break;
        case epoll_op::detach:
            operation = EPOLL_CTL_DEL;
            break;
        default:
            LOG_ERROR("epoll_fd: {} control fd: {} op: {} not support", epoll_fd_, op->fd, static_cast<int>(op_type));
            return -1;
    }
    struct epoll_event event{};
    // data.ptr 直接存 operator 指针。epoll_event 是 packed 的，
    // 编译器按 packed 生成存取，不需要也不能自己 memcpy 绕开
    event.data.ptr = op;
    event.events = events;
    struct epoll_event* event_ptr = operation == EPOLL_CTL_DEL ? nullptr : &event;
    if (handle_eintr(::epoll_ctl, epoll_fd_, operation, op->fd, event_ptr) == -1) {
        if (operation == EPOLL_CTL_ADD && errno == EEXIST) {
            return 0;
        }
        if (operation == EPOLL_CTL_MOD && errno == ENOENT) {
            return 0;
        }
        if (operation == EPOLL_CTL_DEL && (errno == ENOENT || errno == EBADF)) {
            // fd 已经关掉或已从 epoll 中移除，属于正常时序
            return 0;
        }
        LOG_SYSTEM_ERROR("epoll_fd: {} epoll_ctl fd: {} operation: {}", epoll_fd_, op->fd, operation);
        return -1;
    }
    return 0;
}

int epoll::wait() {
    // 固定阻塞。netpoll 有事件后把超时改成 0 是为了让 Go 协程调度器切换，
    // C++ 里 msec = 0 只会变成忙转
    constexpr int timeout = -1;
    LOG_INFO("wait epoll epoll_fd: {} event_fd: {}", epoll_fd_, wake_fd_);
    while (true) {
        const int count = handle_eintr(::epoll_wait, epoll_fd_, events_.data(), static_cast<int>(events_.size()), timeout);
        if (count == -1) {
            // EINTR 已经被 handle_eintr 吃掉，走到这里都是持久性错误，
            // 不能打完日志继续循环，否则会满核空转
            LOG_SYSTEM_ERROR("epoll_wait epoll_fd: {}", epoll_fd_);
            return -1;
        }
        if (handle(count)) {
            // 和 netpoll 一样在这里收尾：Wait() 返回时 eventfd 和 epoll fd 都已经关掉，
            // 调用方不需要再管。放在这里而不是析构里，是为了让"退出"这件事有明确的完成点
            if (control(wake_operator_, epoll_op::detach) == -1) {
                LOG_ERROR("epoll_fd: {} control event_fd: {} detach error", epoll_fd_, wake_fd_);
            }
            const int epoll_fd = epoll_fd_;
            const int wake_fd = wake_fd_;
            // 先置 -1 再关，信号处理函数看到 -1 就不会往已被复用的 fd 号上写
            wake_fd_ = -1;
            epoll_fd_ = -1;
            static_cast<void>(ignore_eintr(::close, wake_fd));
            static_cast<void>(ignore_eintr(::close, epoll_fd));
            trigger_.store(0);
            LOG_INFO("exit gracefully epoll_fd: {} event_fd: {}", epoll_fd, wake_fd);
            return 0;
        }
    }
}

bool epoll::handle(int event_num) {
    // hup 延后到本轮读写都处理完再统一处理：直接按事件位 if-else 会跳过 EPOLLIN，
    // 把对端 shutdown 后还留在内核缓冲区里的数据丢掉
    bool exit = false;
    hups_.clear();
    for (int i = 0; i < event_num; i++) {
        const struct epoll_event& event = events_[i];
        auto* op = static_cast<fd_operator*>(event.data.ptr);
        if (op == nullptr) {
            continue;
        }
        // 和 netpoll 一样放在唤醒判断之前，唤醒事件也打一条
        LOG_INFO("wake epoll epoll_fd: {} client_fd: {} event: {}", epoll_fd_, op->fd, event_string(event.events));
        if (op == wake_operator_) {
            if (handle_wake()) {
                // 只置标志不立刻返回：同一批 epoll_wait 返回的事件里，
                // 排在唤醒事件后面的那些 fd 这一轮也要派发完，不能丢
                exit = true;
            }
            continue;
        }
        if ((event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0 && op->on_hup != nullptr) {
            hups_.push_back(op);
        }
        if ((event.events & EPOLLIN) != 0 && op->on_read != nullptr) {
            op->on_read(this, op);
        }
        if ((event.events & EPOLLOUT) != 0 && op->on_write != nullptr) {
            op->on_write(this, op);
        }
    }
    for (fd_operator* op : hups_) {
        // 先摘掉再回调：on_hup 里会 close(fd)，关掉之后内核自动把它从 epoll 摘除
        if (control(op, epoll_op::detach) == -1) {
            LOG_ERROR("epoll_fd: {} control client_fd: {} detach error", epoll_fd_, op->fd);
        }
        if (op->on_hup != nullptr) {
            op->on_hup(this, op);
        }
    }
    hups_.clear();
    return exit;
}

bool epoll::handle_wake() {
    // eventfd 必须一次读满 8 字节，读 1 字节清不掉计数器，
    // 水平触发下会变成永久可读
    uint64_t value = 0;
    if (handle_eintr([&] { return ::read(wake_fd_, &value, sizeof(value)); }) == -1) {
        if (errno != EAGAIN) {
            LOG_SYSTEM_ERROR("read event_fd: {}", wake_fd_);
        }
        return false;
    }
    return value > 0;
}

void epoll::close() {
    const int fd = wake_fd_;
    if (fd == -1) {
        return;
    }
    // 只让第一次调用生效
    if (trigger_.fetch_add(1) > 0) {
        return;
    }
    const uint64_t value = 1;
    static_cast<void>(ignore_eintr([&] { return ::write(fd, &value, sizeof(value)); }));
}
