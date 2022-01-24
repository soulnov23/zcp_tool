#pragma once

#define SPDLOG_FMT_EXTERNAL

#include <string.h>
#include <unistd.h>

#include <iostream>

#include "fmt/format.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "src/base/singleton.h"
#include "src/base/time_util.h"

#define RED_PRINT_BEG    "\e[1;31m"
#define RED_PRINT_END    "\e[m"
#define GREEN_PRINT_BEG  "\e[1;32m"
#define GREEN_PRINT_END  "\e[m"
#define YELLOW_PRINT_BEG "\e[1;33m"
#define YELLOW_PRINT_END "\e[m"
#define BLUE_PRINT_BEG   "\e[1;34m"
#define BLUE_PRINT_END   "\e[m"

#define CONSOLE_DEBUG(formatter, args...)                                                                           \
    do {                                                                                                            \
        std::cout << fmt::format("[{}] [{}] [" GREEN_PRINT_BEG "debug" GREEN_PRINT_END "] [{}:{} {}()] " formatter, \
                                 get_time_now(), getpid(), __FILE__, __LINE__, __FUNCTION__, ##args)                \
                  << std::endl;                                                                                     \
    } while (0)

#define CONSOLE_ERROR(formatter, args...)                                                                                       \
    do {                                                                                                                        \
        std::cout << fmt::format("[{}] [{}] [" RED_PRINT_BEG "error" RED_PRINT_END "] [{}:{} {}()] " formatter, get_time_now(), \
                                 getpid(), __FILE__, __LINE__, __FUNCTION__, ##args)                                            \
                  << std::endl;                                                                                                 \
    } while (0)

#define CONSOLE_SYSTEM_ERROR(formatter, args...)                                                                             \
    do {                                                                                                                     \
        std::cout << fmt::format("[{}] [{}] [" RED_PRINT_BEG "error" RED_PRINT_END "] [{}:{} {}()] " formatter               \
                                 " errno: {}, errmsg: {}",                                                                   \
                                 get_time_now(), getpid(), __FILE__, __LINE__, __FUNCTION__, ##args, errno, strerror(errno)) \
                  << std::endl;                                                                                              \
    } while (0)

struct logger_config {
    std::string logger_name = "default_logger";
    std::string format = "[%Y-%m-{} %H:%M:%S.%f] [%P] [%^%l%$] [{}:%# %!()] %v";
    std::string filename = "../log/default.log";
    // by_size按大小分割 by_day按天分割 by_hour按小时分割
    std::string roll_type = "by_day";
    unsigned int reserve_count = 10;
    unsigned int roll_size = 1024 * 1024 * 100;  // 默认100 MB
    // 表示按天切割的时刻，具体是时刻通过rotation_hour:rotation_minute指定
    unsigned int rotation_hour = 0;
    unsigned int rotation_minute = 0;
    unsigned int async_thread_pool_size = 1;
};

class logger : public singleton<logger> {
    friend class singleton<logger>;
    CLASS_UNCOPYABLE(logger)
    CLASS_UNMOVABLE(logger)
public:
    logger();
    ~logger();

    int set_config(const logger_config& config);

    void log(const char* filename_in, int line_in, const char* funcname_in, spdlog::level::level_enum level,
             const std::string& msg);

private:
    std::shared_ptr<spdlog::sinks::sink> sink_;
    std::shared_ptr<spdlog::logger> logger_;
    std::shared_ptr<spdlog::details::thread_pool> thread_pool_;
};

#define LOG_IMPL(level, formatter, args...)                                                                          \
    do {                                                                                                             \
        logger::get_instance_atomic()->log(__FILE__, __LINE__, __FUNCTION__, level, fmt::format(formatter, ##args)); \
    } while (0)

#define LOG_DEBUG(format, args...) LOG_IMPL(spdlog::level::debug, format, ##args)
#define LOG_ERROR(format, args...) LOG_IMPL(spdlog::level::err, format, ##args)

#define LOG_SYSTEM_ERROR(formatter, args...)                                                                                 \
    do {                                                                                                                     \
        logger::get_instance_atomic()->log(__FILE__, __LINE__, __FUNCTION__, spdlog::level::err,                             \
                                           fmt::format(formatter " errno: {}, errmsg: {}", ##args, errno, strerror(errno))); \
    } while (0)
