#include "src/base/log.h"

#include <iostream>

#include "spdlog/async_logger.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/hourly_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

logger::logger() {
    try {
        // 设置打印日志时异常处理函数
        spdlog::set_error_handler([](const std::string& msg) {
            std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, msg) << std::endl;
        });

        // 设置console logger
        auto sink_ = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
        auto formatter = std::make_unique<spdlog::pattern_formatter>(DEFAULT_FORMAT, spdlog::pattern_time_type::local,
                                                                     spdlog::details::os::default_eol);
        // unique_ptr不能复制，无法通过值传递调用set_formatter，只能使用move
        sink_->set_formatter(std::move(formatter));
        console_logger_ = std::make_shared<spdlog::logger>("console_logger", std::move(sink_));
        // 设置默认刷新级别和打印级别
        console_logger_->flush_on(spdlog::level::trace);
        console_logger_->set_level(spdlog::level::trace);
        // 注册到spdlog
        spdlog::register_logger(console_logger_);
    } catch (const std::exception& ex) {
        std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, ex.what()) << std::endl;
    } catch (...) {
        std::cout << fmt::format("[{}:{} {}()] unknow exception", __FILE__, __LINE__, __FUNCTION__) << std::endl;
    }

    run_logger_inited_ = false;
}

logger::~logger() { spdlog::drop_all(); }

int logger::set_config(const logger_config& config) {
    int ret = 0;
    do {
        try {
            std::shared_ptr<spdlog::sinks::sink> sink_;
            if (config.roll_type == "by_size") {
                sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config.file_name, config.roll_size,
                                                                               config.reserve_count, true);
            } else if (config.roll_type == "by_day") {
                sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config.file_name, config.rotation_hour,
                                                                            config.rotation_minute, false, config.reserve_count);
            } else if (config.roll_type == "by_hour") {
                sink_ = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(config.file_name, false, config.reserve_count);
            } else {
                ret = 1;
                LOG_ERROR("invalid roll_type: {}", config.roll_type.c_str());
                break;
            }
            auto formatter = std::make_unique<spdlog::pattern_formatter>(config.format, spdlog::pattern_time_type::local,
                                                                         spdlog::details::os::default_eol);
            // unique_ptr不能复制，无法通过值传递调用set_formatter，只能使用move
            sink_->set_formatter(std::move(formatter));
            thread_pool_ = std::make_shared<spdlog::details::thread_pool>(config.async_thread_pool_size, 1);
            run_logger_ = std::make_shared<spdlog::async_logger>(config.name, std::move(sink_), thread_pool_,
                                                                 spdlog::async_overflow_policy::block);
            // 设置默认刷新级别和打印级别
            run_logger_->flush_on(spdlog::level::trace);
            run_logger_->set_level(config.level);
            // 注册到spdlog
            spdlog::register_logger(run_logger_);
        } catch (const std::exception& ex) {
            ret = -1;
            LOG_ERROR("{}", ex.what());
        } catch (...) {
            ret = -1;
            LOG_ERROR("unknow exception");
        }
    } while (0);
    run_logger_inited_ = true;
    return ret;
}

void logger::log(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
                 const std::string& msg) {
    try {
        if (run_logger_inited_) {
            run_logger_->log(spdlog::source_loc{file_name_in, line_in, func_name_in}, level, msg);
        } else {
            console_logger_->log(spdlog::source_loc{file_name_in, line_in, func_name_in}, level, msg);
        }
    } catch (const std::exception& ex) {
        LOG_ERROR("{}", ex.what());
    } catch (...) {
        LOG_ERROR("unknow exception");
    }
}