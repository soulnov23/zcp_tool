#pragma once

#include <unistd.h>

#include "src/base/macros.h"

class fd_guard {
    CLASS_UNCOPYABLE(fd_guard)
    CLASS_UNMOVABLE(fd_guard)

public:
    fd_guard() : fd_(-1) {}
    explicit fd_guard(int fd) : fd_(fd) {}
    ~fd_guard() {
        if (fd_ < 0) {
            return;
        }
        close(fd_);
        fd_ = -1;
    }

    // 支持fd_guard类型到int类型的隐式转换
    operator int() { return fd_; }
    // 支持fd_guard等号赋值int类型
    void operator=(int fd) { fd_ = fd; }
    // 支持fd_guard比较int类型
    bool operator==(int fd) { return fd_ == fd; }

private:
    int fd_;
};