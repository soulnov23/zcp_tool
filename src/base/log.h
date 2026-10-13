#pragma once

#include <errno.h>
#include <string.h>

#include <atomic>
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
    std::string file_name = "../log/run.log";
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

    void log(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
             const std::string& msg);

private:
    std::atomic<std::shared_ptr<spdlog::logger>> logger_;
};

#define LOG_IMPL(level, formatter, args...)                                                              \
    do {                                                                                                 \
        logger::instance().log(__FILE__, __LINE__, __FUNCTION__, level, fmt::format(formatter, ##args)); \
    } while (0)

#define LOG_TRACE(formatter, args...)    LOG_IMPL(spdlog::level::trace, formatter, ##args)
#define LOG_DEBUG(formatter, args...)    LOG_IMPL(spdlog::level::debug, formatter, ##args)
#define LOG_INFO(formatter, args...)     LOG_IMPL(spdlog::level::info, formatter, ##args)
#define LOG_WARN(formatter, args...)     LOG_IMPL(spdlog::level::warn, formatter, ##args)
#define LOG_ERROR(formatter, args...)    LOG_IMPL(spdlog::level::err, formatter, ##args)
#define LOG_CRITICAL(formatter, args...) LOG_IMPL(spdlog::level::critical, formatter, ##args)

// 在消息末尾附上errno和对应的错误描述
#define LOG_SYSTEM_ERROR(formatter, args...)                                                                     \
    do {                                                                                                         \
        logger::instance().log(__FILE__, __LINE__, __FUNCTION__, spdlog::level::err,                             \
                               fmt::format(formatter " errno: {}, errmsg: {}", ##args, errno, strerror(errno))); \
    } while (0)
