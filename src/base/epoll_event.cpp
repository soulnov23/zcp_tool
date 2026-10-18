#include "src/base/epoll_event.h"

std::string event_string(uint32_t events) {
    std::string result;
    // 顺序和 /proc 以及 strace 的习惯保持一致，方便和别的工具的输出对照
    if (events & EPOLLIN) {
        result += "|EPOLLIN";
    }
    if (events & EPOLLOUT) {
        result += "|EPOLLOUT";
    }
    if (events & EPOLLRDHUP) {
        result += "|EPOLLRDHUP";
    }
    if (events & EPOLLHUP) {
        result += "|EPOLLHUP";
    }
    if (events & EPOLLERR) {
        result += "|EPOLLERR";
    }
    if (result.empty()) {
        return result;
    }
    // 去掉拼串时打头的那个分隔符
    return result.substr(1);
}
