#pragma once

#include <sys/epoll.h>

#include <cstdint>
#include <string>

// 读事件关注的位组合，对应 netpoll 的 ReadFlags。
// 刻意不带 EPOLLET：netpoll 全程用水平触发，单次读不完的数据下一轮 epoll_wait
// 还会再报一次。带上 EPOLLET 就必须把每次读写到 EAGAIN 为止，回调的正确性要求高得多
inline constexpr uint32_t read_flags = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR;

// 写事件关注的位组合，对应 netpoll 的 WriteFlags。
// 这里没有 EPOLLRDHUP：只挂写事件时看不到对端的半关闭
inline constexpr uint32_t write_flags = EPOLLOUT | EPOLLHUP | EPOLLERR;

// 读写同时关注。不能直接用 read_flags | write_flags 之外拼出来的组合：
// 至少要保证 EPOLLRDHUP 还在，否则从 mod_writable 切回来时半关闭通知会丢
inline constexpr uint32_t read_write_flags = read_flags | EPOLLOUT;

// 对应 netpoll 的事件枚举。ADD 和 MOD 分开成不同的值，
// 调用方必须显式说明这次是首次注册还是改注册，避免把 ADD 当 MOD 或反过来
enum class epoll_op {
    // EPOLL_CTL_ADD，只关注读
    readable,
    // EPOLL_CTL_MOD，只关注读
    mod_readable,
    // EPOLL_CTL_ADD，只关注写
    writable,
    // EPOLL_CTL_MOD，只关注写
    mod_writable,
    // EPOLL_CTL_ADD，读写都关注
    read_writable,
    // EPOLL_CTL_MOD，读写都关注
    mod_read_writable,
    // EPOLL_CTL_DEL
    detach,
};

// 把事件位翻译成"|EPOLLIN|EPOLLOUT"这样的可读串，日志里用。
// 没有任何已知位时返回空串
[[nodiscard]] std::string event_string(uint32_t events);
