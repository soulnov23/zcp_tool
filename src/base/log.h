#pragma once

#include "fmt/format.h"
#include "spdlog/async_logger.h"
#include "spdlog/common.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/hourly_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "src/base/printf_util.h"
#include "src/base/singleton.h"

struct logger_config {
    std::string logger_name = "default_logger";
    std::string format = "[%Y-%m-%d %H:%M:%S.%f] [process %P] [%^%l%$] [%s:%# %!()] %v";
    std::string filename = "default.log";
    // by_size按大小分割 by_day按天分割 by_hour按小时分割
    std::string roll_type = "by_day";
    unsigned int reserve_count = 10;
    unsigned int roll_size = 1024 * 1024 * 100;  // 默认100 MB
    // 表示按天切割的时刻，具体是时刻通过rotation_hour:rotation_minute指定
    unsigned int rotation_hour = 0;
    unsigned int rotation_minute = 0;
    unsigned int async_thread_pool_size = 1;

    void debug() const {
        PRINTF_DEBUG(
            "formatter: %s, filename: %s, roll_type: %s, reserve_count: %u, roll_size: %u, rotation_hour: %u, "
            "rotation_minute: %u, async_thread_pool_size: %u",
            format.c_str(), filename.c_str(), roll_type.c_str(), reserve_count, roll_size, rotation_hour, rotation_minute,
            async_thread_pool_size);
    }
};

logger_config default_logger_config;

class logger : public singleton<logger> {
    friend class singleton<logger>;
    CLASS_UNCOPYABLE(logger)
    CLASS_UNMOVABLE(logger)
public:
    logger() { set_config(default_logger_config); }
    ~logger() { spdlog::drop_all(); }

    int set_config(const logger_config& config = default_logger_config) {
        if (config.roll_type == "by_size") {
            sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config.filename, config.roll_size,
                                                                           config.reserve_count, true);
        } else if (config.roll_type == "by_day") {
            sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config.filename, config.rotation_hour,
                                                                        config.rotation_minute, false, config.reserve_count);
        } else if (config.roll_type == "by_hour") {
            sink_ = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(config.filename, false, config.reserve_count);
        } else {
            PRINTF_DEBUG("roll_type: %s err", config.roll_type.c_str());
            return -1;
        }
        auto formatter = std::make_unique<spdlog::pattern_formatter>(config.format, spdlog::pattern_time_type::local,
                                                                     spdlog::details::os::default_eol);
        sink_->set_formatter(std::move(formatter));
        thread_pool_ = std::make_shared<spdlog::details::thread_pool>(config.async_thread_pool_size, 1);
        logger_ =
            std::make_shared<spdlog::async_logger>(config.logger_name, sink_, thread_pool_, spdlog::async_overflow_policy::block);
        // 设置默认刷新级别和打印级别
        logger_->flush_on(spdlog::level::trace);
        logger_->set_level(spdlog::level::trace);
        // 注册到spdlog
        spdlog::register_logger(logger_);
        return 0;
    }

    void log(const char* filename_in, int line_in, const char* funcname_in, spdlog::level::level_enum level,
             const std::string& msg) {
        logger_->log(spdlog::source_loc{filename_in, line_in, funcname_in}, level, msg);
    }

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