#pragma once

#include <unistd.h>

#include "src/base/macros.h"

class fd_guard {
    CLASS_UNCOPYABLE(fd_guard)
    CLASS_UNMOVABLE(fd_guard)

public:
    explicit fd_guard(int fd) : fd_(fd) {}

    ~fd_guard() {
        close(fd_);
        fd_ = -1;
    }

    // 支持fd_guard类型到int类型的隐式转换
    operator int() { return fd_; }

private:
    int fd_;
};