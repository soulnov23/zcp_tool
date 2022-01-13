#include "src/base/log.h"

#include "spdlog/async_logger.h"
#include "spdlog/common.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/hourly_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

logger::logger() {
    // 设置打印日志时异常处理函数
    spdlog::set_error_handler([](const std::string& msg) { CONSOLE_ERROR("{}", msg); });
    logger_config default_logger_config;
    // 默认配置初始化不会有问题，不处理返回值
    set_config(default_logger_config);
}

logger::~logger() { spdlog::drop_all(); }

int logger::set_config(const logger_config& config) {
    try {
        if (config.roll_type == "by_size") {
            sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config.filename, config.roll_size,
                                                                           config.reserve_count, true);
        } else if (config.roll_type == "by_day") {
            sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config.filename, config.rotation_hour,
                                                                        config.rotation_minute, false, config.reserve_count);
        } else if (config.roll_type == "by_hour") {
            sink_ = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(config.filename, false, config.reserve_count);
        } else {
            CONSOLE_DEBUG("roll_type: %s err", config.roll_type.c_str());
            return 1;
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
    } catch (const std::exception& ex) {
        CONSOLE_ERROR("{}", ex.what());
        return -1;
    } catch (...) {
        CONSOLE_ERROR("unknow exception");
        return -1;
    }
    return 0;
}

void logger::log(const char* filename_in, int line_in, const char* funcname_in, spdlog::level::level_enum level,
                 const std::string& msg) {
    try {
        logger_->log(spdlog::source_loc{filename_in, line_in, funcname_in}, level, msg);
    } catch (const std::exception& ex) {
        CONSOLE_ERROR("{}", ex.what());
    } catch (...) {
        CONSOLE_ERROR("unknow exception");
    }
}