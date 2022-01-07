#include "src/base/file_util.h"

#include <fcntl.h>

#include <fstream>
#include <streambuf>

#include "src/base/fd_guard.h"
#include "src/base/fd_lock_guard.h"
#include "src/base/printf_util.h"

std::string file_to_string(const char* file_path) {
    std::string data;
    std::ifstream file(file_path);
    if (!file.is_open()) {
        PRINTF_ERROR("open file failed : %s", file_path);
        return data;
    }
    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return data;
}

int string_to_file(const char* file_path, const string& data) {
    fd_guard fd(open(file_path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU));
    if (fd == -1) {
        PRINTF_ERROR("open %s err", file_path);
        return -1;
    }
    fd_lock_guard fd_lock(fd);
    if (fd_lock.lock(false) == -1) {
        PRINTF_ERROR("fd_lock.lock err");
        return -1;
    }
    // 这里是阻塞写入文件，不需要用while循环
    ssize_t size = write(fd, data.c_str(), data.size());
    if (size != data.size()) {
        PRINTF_ERROR("write err");
        return -1;
    }
    return 0;
}