#include "src/base/fd_lock_guard.h"

#include <errno.h>
#include <fcntl.h>

#include "src/base/log.h"

fd_lock_guard::fd_lock_guard() : fd_(-1) {}

fd_lock_guard::fd_lock_guard(int fd) : fd_(fd) {}

fd_lock_guard::~fd_lock_guard() { unlock(); };

fd_lock_guard::operator int() { return fd_; }

void fd_lock_guard::operator=(int fd) { fd_ = fd; }

bool fd_lock_guard::operator==(int fd) { return fd_ == fd; }

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
                LOG_ERROR("fcntl lock by other process");
                return -2;
            }
            LOG_SYSTEM_ERROR("fcntl fd: {}", fd_);
            return -1;
        }
    } else {
        // 阻塞用来判断是否写
        if (-1 == fcntl(fd_, F_SETLKW, &fd_lock)) {
            LOG_SYSTEM_ERROR("fcntl fd: {}", fd_);
            return -1;
        }
    }
    return 0;
}

void fd_lock_guard::unlock() {
    struct flock fd_lock;
    fd_lock.l_type = F_UNLCK;
    fd_lock.l_whence = SEEK_SET;
    fd_lock.l_start = 0;
    fd_lock.l_len = 0;
    fcntl(fd_, F_SETLK, &fd_lock);
}