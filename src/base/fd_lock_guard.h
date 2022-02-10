#pragma once

#include "src/base/macros.h"

class fd_lock_guard {
    CLASS_UNCOPYABLE(fd_lock_guard)
    CLASS_UNMOVABLE(fd_lock_guard)

public:
    fd_lock_guard();
    explicit fd_lock_guard(int fd);
    ~fd_lock_guard();

    // 支持fd_lock_guard类型到int类型的隐式转换
    operator int();
    // 支持fd_lock_guard等号赋值int类型
    void operator=(int fd);
    // 支持fd_lock_guard比较int类型
    bool operator==(int fd);

    int lock(bool non_blocking = true);
    void unlock();

private:
    int fd_;
};