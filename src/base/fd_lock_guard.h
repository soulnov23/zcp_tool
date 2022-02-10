#pragma once

#include "src/base/macros.h"

class fd_lock_guard {
    CLASS_UNCOPYABLE(fd_lock_guard)
    CLASS_UNMOVABLE(fd_lock_guard)

public:
    fd_lock_guard();
    explicit fd_lock_guard(int fd);
    ~fd_lock_guard();

    // 支持fd_lock_guard等号赋值int类型
    void operator=(int fd);

    int lock(bool non_blocking = true);
    void unlock();

private:
    int fd_;
};