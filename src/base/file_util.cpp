#include "src/base/file_util.h"

#include <fstream>
#include <ios>
#include <streambuf>

#include "src/base/eintr.h"
#include "src/base/log.h"

off_t get_file_size(const char* file_path) {
    struct stat64 temp_stat;
    if (handle_eintr(stat64, file_path, &temp_stat) == -1) {
        LOG_SYSTEM_ERROR("stat64");
        return -1;
    }
    return temp_stat.st_size;
}

// 失败返回空串。空串无法和空文件区分，调用方需要区分时改用std::optional
std::string load_file_data(const char* file_path) {
    std::string data;
    // binary：不做文本模式的换行转换，读出来的就是磁盘上的字节
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        // ifstream打开失败不保证设置errno，这里不能用LOG_SYSTEM_ERROR
        LOG_ERROR("open file({}) failed", file_path);
        return data;
    }
    // 目录也能open成功，is_open()拦不住，读的时候filebuf::underflow才抛异常。
    // 异常由filebuf直接抛出，不经过流的异常掩码，exceptions()屏蔽不掉，只能catch
    try {
        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    } catch (const std::ios_base::failure& e) {
        LOG_ERROR("read file({}) failed: {}", file_path, e.what());
        return {};
    }
    if (file.bad()) {  // 读到一半出错，data是截断的，不能当成功返回
        LOG_ERROR("read file({}) incomplete", file_path);
        return {};
    }
    return data;
}