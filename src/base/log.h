#pragma once

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "src/base/singleton.h"

class logger : public singleton<logger> {
public:
    logger();
    ~logger();

private:
    spdlog::sink_ptr sink_;
};