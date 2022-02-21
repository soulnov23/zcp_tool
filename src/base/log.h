#pragma once

#define SPDLOG_FMT_EXTERNAL

#include <string.h>
#include <unistd.h>

#include "fmt/format.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "src/base/singleton.h"

#define DEFAULT_FORMAT "[%Y-%m-%d %H:%M:%S.%f] [%P] [%^%l%$] [%s:%# %!()] %v"

struct logger_config {
    std::string name = "run_logger";
    spdlog::level::level_enum level = spdlog::level::trace;
    std::string format = DEFAULT_FORMAT;
    std::string file_name = "../log/run.log";
    // by_size按大小分割 by_day按天分割 by_hour按小时分割
    std::string roll_type = "by_day";
    unsigned int reserve_count = 10;
    unsigned int roll_size = 1024 * 1024 * 100;  // 默认100 MB
    // 表示按天切割的时刻，具体是时刻通过rotation_hour:rotation_minute指定
    unsigned int rotation_hour = 0;
    unsigned int rotation_minute = 0;
    // 异步日志线程池大小
    unsigned int async_thread_pool_size = 1;
};

class logger : public singleton<logger> {
    friend class singleton<logger>;
    CLASS_UNCOPYABLE(logger)
    CLASS_UNMOVABLE(logger)
public:
    logger();
    ~logger();

    // 在构造函数中创建了console作为默认打印，调用set_config后改为file打印
    int set_config(const logger_config& config);

    void log(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
             const std::string& msg);

private:
    std::shared_ptr<spdlog::logger> console_logger_;
    std::shared_ptr<spdlog::logger> run_logger_;
    std::shared_ptr<spdlog::details::thread_pool> thread_pool_;
    bool run_logger_inited_;
};

#define LOG_IMPL(level, formatter, args...)                                                                   \
    do {                                                                                                      \
        logger::get_instance()->log(__FILE__, __LINE__, __FUNCTION__, level, fmt::format(formatter, ##args)); \
    } while (0)

#define LOG_DEBUG(format, args...) LOG_IMPL(spdlog::level::debug, format, ##args)
#define LOG_ERROR(format, args...) LOG_IMPL(spdlog::level::err, format, ##args)

#define LOG_SYSTEM_ERROR(formatter, args...)                                                                          \
    do {                                                                                                              \
        logger::get_instance()->log(__FILE__, __LINE__, __FUNCTION__, spdlog::level::err,                             \
                                    fmt::format(formatter " errno: {}, errmsg: {}", ##args, errno, strerror(errno))); \
    } while (0)
