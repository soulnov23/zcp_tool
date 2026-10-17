#pragma once

#include <signal.h>

#include <cstdint>
#include <string>

#include "src/base/fd_lock_guard.h"
#include "src/base/log.h"
#include "src/base/macros.h"
#include "src/base/singleton.h"

namespace YAML {
    class Node;
}

struct server_config {
    std::string name;
    uint8_t process_num;
    std::string ip;
    uint16_t port;
    uint16_t backlog;
    uint16_t event_num;
};
struct app_config {
    server_config server;
    // 直接复用日志模块的配置结构，不再在这里重复一份
    logger_config log;
};

class app : public singleton<app> {
    friend class singleton<app>;

private:
    // 构造和析构私有，实例只能通过singleton::instance()获取
    app();
    ~app();

public:
    static void signal_handler_func(int sig_no, siginfo_t* sig_info, void* data);

    int start(int argc, char* argv[]);

private:
    int get_option(int argc, char* argv[]);
    void show_help_info();
    void show_version_info();
    int signal_process();
    int init_signal();
    int config_process();
    int parse_log_config(const YAML::Node& log_node);
    int daemon_process();
    int single_process();
    int log_process();
    int create_pid_file();
    int fork_child();
    void check();

private:
    std::string pid_file_;
    std::string lock_file_;
    bool show_help_;
    bool show_version_;
    std::string config_file_;
    std::string signal_cmd_;
    fd_lock_guard single_process_;
    app_config config_;
};