#include "src/base/log.h"

#include <cstdlib>
#include <iostream>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

static std::shared_ptr<spdlog::logger> make_logger(const logger_config& config) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    console_sink->set_pattern(config.format);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(config.file_name);
    file_sink->set_pattern(config.format);

    auto new_logger = std::make_shared<spdlog::logger>(config.name, spdlog::sinks_init_list{console_sink, file_sink});
    new_logger->set_level(config.level);
    new_logger->flush_on(spdlog::level::trace);
    return new_logger;
}

logger::logger() {
    try {
        // 设置打印日志时异常处理函数
        spdlog::set_error_handler([](const std::string& msg) {
            std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, msg) << std::endl;
        });
        // 用默认配置先把console和file建起来，set_config之前的日志也不会丢
        logger_.store(make_logger(logger_config()));
    } catch (const std::exception& ex) {
        std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, ex.what()) << std::endl;
        ::exit(1);
    } catch (...) {
        std::cout << fmt::format("[{}:{} {}()] unknow exception", __FILE__, __LINE__, __FUNCTION__) << std::endl;
        ::exit(1);
    }
}

int logger::set_config(const logger_config& config) {
    try {
        // 构造成功后再整体替换，中途失败不影响已有的输出
        logger_.store(make_logger(config));
    } catch (const std::exception& ex) {
        LOG_ERROR("{}", ex.what());
        return -1;
    } catch (...) {
        LOG_ERROR("unknow exception");
        return -1;
    }
    return 0;
}

void logger::log(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
                 const std::string& msg) {
    try {
        logger_.load()->log(spdlog::source_loc{file_name_in, line_in, func_name_in}, level, msg);
    } catch (const std::exception& ex) {
        std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, ex.what()) << std::endl;
    } catch (...) {
        std::cout << fmt::format("[{}:{} {}()] unknow exception", __FILE__, __LINE__, __FUNCTION__) << std::endl;
    }
}
