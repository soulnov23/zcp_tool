#pragma once

#include <atomic>
#include <cerrno>
#include <cstring>
#include <memory>
#include <string>

#include "spdlog/common.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"
#include "src/base/singleton.h"

#define DEFAULT_FORMAT "[%Y-%m-%d %H:%M:%S.%e] [%P] [%^%l%$] [%s:%# %!] %v"

struct logger_config {
    std::string name = "run";
    spdlog::level::level_enum level = spdlog::level::trace;
    std::string format = DEFAULT_FORMAT;
    std::string file_name = "run.log";
};

// 日志单例必须活到其它静态对象析构之后，故采用no_destroy策略，析构函数不会执行
class logger : public singleton<logger, singleton_policy::no_destroy> {
    friend class singleton<logger, singleton_policy::no_destroy>;

private:
    // 构造和析构私有，实例只能通过singleton::instance()获取。
    // 构造时就按logger_config的默认值把console和file建好，建不起来直接退出进程
    logger();
    ~logger() = default;

public:
    // 换成新配置，失败返回-1且保持原有输出不变
    int set_config(const logger_config& config);

    // formatter是编译期检查的格式串，实参类型不符、占位符数量不对都会编译报错。
    // 格式化本身在vlog内部完成而不是在调用点，原因见vlog的实现注释
    template <typename... Args>
    void log(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
             spdlog::format_string_t<Args...> formatter, Args&&... args) {
        // args在函数内是具名左值，不能forward：fmt::make_format_args的形参是左值引用。
        // 实参本体的生命期覆盖调用点整条full-expression，故擦除类型后的view在vlog里仍有效
        vlog(file_name_in, line_in, func_name_in, level, formatter, fmt::make_format_args(args...));
    }

private:
    // 擦掉实参类型，把格式化和异常处理收敛到cpp里的一个函数，
    // 避免每个调用点按实参类型各实例化一份格式化代码
    void vlog(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
              fmt::string_view formatter, fmt::format_args args);

    std::atomic<std::shared_ptr<spdlog::logger>> logger_;
};

#define LOG_IMPL(level, formatter, ...)                                                                        \
    do {                                                                                                       \
        logger::instance().log(__FILE__, __LINE__, __FUNCTION__, level, formatter __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)

#define LOG_TRACE(formatter, ...)    LOG_IMPL(spdlog::level::trace, formatter __VA_OPT__(, ) __VA_ARGS__)
#define LOG_DEBUG(formatter, ...)    LOG_IMPL(spdlog::level::debug, formatter __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO(formatter, ...)     LOG_IMPL(spdlog::level::info, formatter __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARN(formatter, ...)     LOG_IMPL(spdlog::level::warn, formatter __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(formatter, ...)    LOG_IMPL(spdlog::level::err, formatter __VA_OPT__(, ) __VA_ARGS__)
#define LOG_CRITICAL(formatter, ...) LOG_IMPL(spdlog::level::critical, formatter __VA_OPT__(, ) __VA_ARGS__)

#define LOG_SYSTEM_ERROR(formatter, ...)                                                                       \
    do {                                                                                                       \
        const int log_saved_errno = errno;                                                                     \
        logger::instance().log(__FILE__, __LINE__, __FUNCTION__, spdlog::level::err,                           \
                               formatter " errno: {}, errmsg: {}" __VA_OPT__(, ) __VA_ARGS__, log_saved_errno, \
                               std::strerror(log_saved_errno));                                                \
    } while (0)
