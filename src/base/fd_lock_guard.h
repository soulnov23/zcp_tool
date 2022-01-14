#pragma once

#include "src/base/macros.h"

class fd_lock_guard {
    CLASS_UNCOPYABLE(fd_lock_guard)
    CLASS_UNMOVABLE(fd_lock_guard)

public:
    explicit fd_lock_guard(int fd);
    ~fd_lock_guard();

    int lock(bool non_blocking = true);
    int unlock();

private:
    int fd_;
};