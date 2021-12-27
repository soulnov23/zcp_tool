#pragma once

#include "src/base/macros.h"
#include "src/base/singleton.h"

struct server_config {
    std::string name;
    int process_num;
    std::string ip;
    uint16_t port;
    int backlog;
};
struct log_config {
    std::string file_name;
};
struct system_config {
    server_config server;
    log_config log;
};

class app : public singleton<app> {
    friend class singleton<app>;
    CLASS_UNCOPYABLE(app)
    CLASS_UNMOVABLE(app)

    app() = default;
    ~app() = default;

public:
    int start(int argc, char* argv[]);

private:
    int read_config(const char* file_path);
    int fork_child();

private:
    system_config config_;
};