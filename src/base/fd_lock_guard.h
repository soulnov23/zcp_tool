#pragma once

#include <errno.h>
#include <fcntl.h>

#include "src/base/macros.h"
#include "src/base/printf_utils.h"

class fd_lock_guard {
    CLASS_UNCOPYABLE(fd_lock_guard)
    CLASS_UNMOVABLE(fd_lock_guard)

public:
    explicit fd_lock_guard(int fd) : fd_(fd) {}
    ~fd_lock_guard() {
        // 解锁除非系统调用失败，否则都是成功
        unlock();
    };

    int lock(bool non_blocking = true) {
        struct flock fd_lock;
        fd_lock.l_type = F_WRLCK;
        fd_lock.l_whence = SEEK_SET;
        fd_lock.l_start = 0;
        fd_lock.l_len = 0;
        if (non_blocking) {
            // 非阻塞用来判断是否加锁
            if (-1 == fcntl(fd_, F_SETLK, &fd_lock)) {
                if (errno == EACCES || errno == EAGAIN) {
                    PRINTF_ERROR("fcntl lock by other process");
                    return -2;
                }
                PRINTF_ERROR("fcntl err");
                return -1;
            }
        } else {
            // 阻塞用来判断是否写
            if (-1 == fcntl(fd_, F_SETLKW, &fd_lock)) {
                PRINTF_ERROR("fcntl err");
                return -1;
            }
        }
        return 0;
    }
    int unlock() {
        struct flock fd_lock;
        fd_lock.l_type = F_UNLCK;
        fd_lock.l_whence = SEEK_SET;
        fd_lock.l_start = 0;
        fd_lock.l_len = 0;
        if (-1 == fcntl(fd_, F_SETLK, &fd_lock)) {
            PRINTF_ERROR("fcntl err");
            return -1;
        }
        return 0;
    }

private:
    int fd_;
};