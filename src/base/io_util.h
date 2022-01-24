#pragma once

#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>

inline void incr(ssize_t) {}
inline void incr(ssize_t n, loff_t& offset) { offset += static_cast<loff_t>(n); }

// wrap call to read/pread/write/pwrite(int __fd, void *__buf, size_t __nbytes, __off_t __offset)
template <typename function_t, typename... offset_t>
ssize_t wrap(function_t func, int fd, void* data, size_t size, offset_t... offset) {
    char* buf = static_cast<char*>(data);
    ssize_t total = 0;
    ssize_t n;
    do {
        n = func(fd, buf, size, offset...);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += n;
        buf += n;
        size -= n;
        incr(n, offset...);
    } while (n != 0 && size);  // n=0或者size=0数据写完了
    return total;
}

// wrap call to readv/preadv/writev/pwritev(int __fd, const iovec *__iovec, int __count, loff_t __offset)
template <typename function_t, typename... offset_t>
ssize_t wrapv(function_t func, int fd, iovec* iov, int count, offset_t... offset) {
    iovec* iov_base = static_cast<iovec*>(iov->iov_base);
    ssize_t total = 0;
    ssize_t n;
    do {
        n = func(fd, iov_base, iov->iov_len, offset...);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += n;
        incr(n, offset...);
        while (n != 0 && count != 0) {
            if (n >= ssize_t(iov->iov_len)) {
                n -= ssize_t(iov->iov_len);
                ++iov;
                --count;
            } else {
                iov->iov_base = static_cast<char*>(iov->iov_base) + n;
                iov->iov_len -= n;
                n = 0;
            }
        }
    } while (count);
    return total;
}