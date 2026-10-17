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
    new_logger->set_error_handler([](const std::string& msg) {
        std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, msg) << std::endl;
    });
    return new_logger;
}

logger::logger() {
    try {
        // 用默认配置先把console和file建起来，set_config之前的日志也不会丢
        logger_.store(make_logger(logger_config()));
    } catch (const std::exception& ex) {
        std::cout << fmt::format("[{}:{} {}()] {}", __FILE__, __LINE__, __FUNCTION__, ex.what()) << std::endl;
        // 用abort而不是exit：本构造函数可能在别的静态对象的动态初始化过程中被触发
        // （那个对象的构造函数里打了第一条日志），此时exit会去跑atexit链、析构那些
        // 初始化尚未完成的静态对象，是未定义行为。abort不做任何收尾，还能留下core。
        // 上面的endl已经把原因冲出去了
        std::abort();
    } catch (...) {
        std::cout << fmt::format("[{}:{} {}()] unknow exception", __FILE__, __LINE__, __FUNCTION__) << std::endl;
        std::abort();
    }
}

int logger::set_config(const logger_config& config) {
    try {
        // 构造成功后再整体替换，中途失败不影响已有的输出。
        // 旧logger由仍持有它的线程用完后自行释放，这是atomic<shared_ptr>在这里的意义
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

void logger::vlog(const char* file_name_in, int line_in, const char* func_name_in, spdlog::level::level_enum level,
                  fmt::string_view formatter, fmt::format_args args) {
    try {
        // 格式化必须放在try里面。fmt在动态宽度/精度越界、用户自定义formatter抛异常、
        // bad_alloc等情况下会在运行期抛，而之前它是在调用点作为实参求值的，
        // 异常会越过这里的try直接窜进业务代码——日志语句不该把异常漏给调用方
        logger_.load()->log(spdlog::source_loc{file_name_in, line_in, func_name_in}, level, fmt::vformat(formatter, args));
    } catch (const std::exception& ex) {
        // 报出的是出问题的那条日志的位置，而不是log.cpp自己的位置
        std::cout << fmt::format("[{}:{} {}()] log failed: {}", file_name_in, line_in, func_name_in, ex.what()) << std::endl;
    } catch (...) {
        // 落盘环节的std::exception由spdlog自己转交给error handler，不会到这里；
        // 但非std::exception它是先转交再重抛的（logger.h:41-43），必须在这里兜住
        std::cout << fmt::format("[{}:{} {}()] log failed: unknow exception", file_name_in, line_in, func_name_in) << std::endl;
    }
}
