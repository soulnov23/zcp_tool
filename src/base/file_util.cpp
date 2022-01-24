#include "src/base/file_util.h"

#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#include <fstream>
#include <streambuf>

#include "src/base/fd_guard.h"
#include "src/base/fd_lock_guard.h"
#include "src/base/io_util.h"
#include "src/base/log.h"
#include "src/base/retry_do.h"

off_t get_file_size(const char* file_path) {
    struct stat64 temp_stat;
    if (retry_do(stat64, file_path, &temp_stat) == -1) {
        LOG_SYSTEM_ERROR("stat64");
        return -1;
    }
    return temp_stat.st_size;
}

std::string load_file_data(const char* file_path) {
    std::string data;
    std::ifstream file(file_path);
    if (!file.is_open()) {
        LOG_SYSTEM_ERROR("ifstream file({})", file_path);
        return data;
    }
    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return data;
}

ssize_t read_file(int fd, void* data, size_t size, loff_t offset) {
    ssize_t ret = wrap(pread64, fd, data, size, offset);
    if (ret == -1) {
        LOG_SYSTEM_ERROR("pread64");
    }
    return ret;
}

ssize_t write_file(int fd, void* data, size_t size, loff_t offset) {
    ssize_t ret = wrap(pwrite64, fd, data, size, offset);
    if (ret == -1) {
        LOG_SYSTEM_ERROR("pwrite64");
    }
    return ret;
}

ssize_t readv_file(int fd, iovec* iov, int count, loff_t offset) {
    ssize_t ret = wrapv(preadv64, fd, iov, count, offset);
    if (ret == -1) {
        LOG_SYSTEM_ERROR("preadv64");
    }
    return ret;
}

ssize_t writev_file(int fd, iovec* iov, int count, loff_t offset) {
    ssize_t ret = wrapv(pwritev64, fd, iov, count, offset);
    if (ret == -1) {
        LOG_SYSTEM_ERROR("pwritev64");
    }
    return ret;
}