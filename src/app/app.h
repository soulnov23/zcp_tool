#pragma once

#include <signal.h>

#include <cstdint>
#include <string>

#include "src/base/log.h"
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
    app();
    ~app();

public:
    // master 的信号处理函数：只置一个 sig_atomic_t 标志，主循环里再转发给子进程
    static void master_signal_handler(int sig_no, siginfo_t* sig_info, void* data);

    // worker 的信号处理函数：收到退出信号时往 eventfd 写 8 字节，唤醒 epoll
    static void worker_signal_handler(int sig_no, siginfo_t* sig_info, void* data);

    int start(int argc, char* argv[]);

private:
    int config_process(const std::string& config_file);
    int parse_log_config(const YAML::Node& log_node);
    int daemon_process();
    int init_master_signal();
    int init_worker_signal();
    int fork_worker();
    void run();
    void forward_signal(int sig_no);
    void reap_worker();

private:
    std::string config_file_;
    app_config config_;
};
