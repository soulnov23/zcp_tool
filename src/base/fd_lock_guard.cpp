#include "src/base/fd_lock_guard.h"

#include <errno.h>
#include <fcntl.h>

#include "src/base/log.h"

fd_lock_guard::fd_lock_guard(int fd) : fd_(fd) {}

fd_lock_guard::~fd_lock_guard() {
    // 解锁除非系统调用失败，否则都是成功
    unlock();
};

int fd_lock_guard::lock(bool non_blocking) {
    struct flock fd_lock;
    fd_lock.l_type = F_WRLCK;
    fd_lock.l_whence = SEEK_SET;
    fd_lock.l_start = 0;
    fd_lock.l_len = 0;
    if (non_blocking) {
        // 非阻塞用来判断是否加锁
        if (-1 == fcntl(fd_, F_SETLK, &fd_lock)) {
            if (errno == EACCES || errno == EAGAIN) {
                CONSOLE_ERROR("fcntl lock by other process");
                return -2;
            }
            CONSOLE_ERROR("fcntl err");
            return -1;
        }
    } else {
        // 阻塞用来判断是否写
        if (-1 == fcntl(fd_, F_SETLKW, &fd_lock)) {
            CONSOLE_ERROR("fcntl err");
            return -1;
        }
    }
    return 0;
}

int fd_lock_guard::unlock() {
    struct flock fd_lock;
    fd_lock.l_type = F_UNLCK;
    fd_lock.l_whence = SEEK_SET;
    fd_lock.l_start = 0;
    fd_lock.l_len = 0;
    if (-1 == fcntl(fd_, F_SETLK, &fd_lock)) {
        CONSOLE_ERROR("fcntl err");
        return -1;
    }
    return 0;
}