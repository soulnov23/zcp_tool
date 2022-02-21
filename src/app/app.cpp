#include "src/app/app.h"

#include <getopt.h>
#include <sys/wait.h>

#include <iostream>

#include "src/app/server.h"
#include "src/base/fd_guard.h"
#include "src/base/fd_lock_guard.h"
#include "src/base/file_util.h"
#include "src/base/log.h"
#include "src/base/net_util.h"
#include "src/base/yaml_parser.h"

struct signal_hander {
    int sig_no;
    std::string sig_name;
    std::string signal_cmd;
    void (*handler)(int, siginfo_t*, void*);
};

signal_hander signals[] = {{SIGHUP, "SIGHUP", "reload", app::signal_handler_func},
                           {SIGINT, "SIGINT", "", app::signal_handler_func},
                           {SIGQUIT, "SIGQUIT", "quit", app::signal_handler_func},
                           {SIGUSR1, "SIGUSR1", "reopen", app::signal_handler_func},
                           {SIGUSR2, "SIGUSR2", "", app::signal_handler_func},
                           {SIGPIPE, "SIGPIPE", "", app::signal_handler_func},
                           {SIGTERM, "SIGTERM", "stop", app::signal_handler_func},
                           {SIGCHLD, "SIGCHLD", "", app::signal_handler_func},
                           {0, "", "", nullptr}};

void app::signal_handler_func(int sig_no, siginfo_t* sig_info, void* data) {
    if (sig_no == SIGHUP) {
        LOG_DEBUG("recv signal:SIGHUP");
    } else if (sig_no == SIGINT) {
        LOG_DEBUG("recv signal:SIGINT");
    } else if (sig_no == SIGQUIT) {
        LOG_DEBUG("recv signal:SIGQUIT");
    } else if (sig_no == SIGUSR1) {
        LOG_DEBUG("recv signal:SIGUSR1");
    } else if (sig_no == SIGUSR2) {
        LOG_DEBUG("recv signal:SIGUSR2");
    } else if (sig_no == SIGPIPE) {
        LOG_DEBUG("recv signal:SIGPIPE");
    } else if (sig_no == SIGTERM) {
        LOG_DEBUG("recv signal:SIGTERM");
    } else if (sig_no == SIGCHLD) {
        LOG_DEBUG("recv signal:SIGCHLD");
        while (true) {
            int status = 0;
            __pid_t pid = waitpid(-1, &status, WNOHANG);
            //设置了WNOHANG没有子进程了
            if (pid == 0) {
                break;
            } else if (pid == -1) {
                //设置了WNOHANG没有子进程了
                if (errno == ECHILD) {
                    break;
                } else {
                    if (errno == EINTR) {
                        continue;
                    }
                    LOG_SYSTEM_ERROR("waitpid");
                    break;
                }
            }
            //成功回收pid>0
            int result = WIFEXITED(status);
            if (result == 0) {
                //正常退出
                LOG_DEBUG("pid: {} normal exit return 0", pid);
            } else {
                //异常退出
                int return_result = WEXITSTATUS(status);
                LOG_DEBUG("pid: {} abnormal exit return {}", pid, return_result);
            }
        }
    } else {
        LOG_DEBUG("no set signal: {}", sig_no);
    }
}

app::app() : pid_file_("zcp_tool.pid"), lock_file_("zcp_tool.lock"), show_help_(false), show_version_(false) {}

app::~app() {}

int app::start(int argc, char* argv[]) {
#ifdef DEBUG
    LOG_DEBUG("this is debug version");
#endif
    if (get_option(argc, argv) != 0) {
        LOG_ERROR("get option error");
        return -1;
    }
    if (show_version_) {
        show_version_info();
    }
    if (show_help_) {
        show_help_info();
    }
    if (!signal_cmd_.empty()) {
        if (signal_process() != 0) {
            LOG_ERROR("signal process error");
            return -1;
        }
    }
    if (init_signal() != 0) {
        LOG_ERROR("init signal error");
        return -1;
    }
    if (config_process() != 0) {
        LOG_ERROR("config process error");
        return -1;
    }
    if (daemon_process() != 0) {
        LOG_ERROR("daemon process error");
        return -1;
    }
    if (single_process() != 0) {
        LOG_ERROR("single process error");
        return -1;
    }
    if (create_pid_file() != 0) {
        LOG_ERROR("create pid file error");
        return -1;
    }
    // 至少创建一个子进程
    int n = config_.server.process_num;
    do {
        if (fork_child() != 0) {
            LOG_ERROR("fork child error");
            return -1;
        }
        n--;
    } while (n > 0);
    while (true) {
        check();
    }
    return 0;
}

int app::get_option(int argc, char* argv[]) {
    while (true) {
        static struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                               {"version", no_argument, nullptr, 'v'},
                                               {"conf", required_argument, nullptr, 'c'},
                                               {"signal", required_argument, nullptr, 's'},
                                               {0, 0, 0, 0}};
        //关闭getopt_long向stderr打印错误信息
        opterr = 0;
        int option_index = 0;
        int character = getopt_long(argc, argv, ":hvc:s:", long_options, &option_index);
        if (character == -1) {
            //全部解析完成
            break;
        }
        switch (character) {
            case '?':
                LOG_ERROR("invalid option: -{}", char(optopt));
                return -1;
            case 'h':
                show_help_ = true;
                break;
            case 'v':
                show_version_ = true;
                break;
            case 'c':
                config_file_ = optarg;
                break;
            case 's':
                signal_cmd_ = optarg;
                if (signal_cmd_ != "stop" || signal_cmd_ != "quit" || signal_cmd_ != "reopen" || signal_cmd_ != "reload") {
                    LOG_ERROR("invalid signal: {}", signal_cmd_);
                    return -1;
                }
                break;
            case ':':
                LOG_ERROR("requires parameter: -{}", char(optopt));
                return -1;
            default:
                break;
        }
    }
    return 0;
}

void app::show_help_info() {
    std::cout << "Usage: zcp_tool [-h] [-v] [-c filename] [-s signal]" << endl
              << "Options:" << endl
              << "  -h,--help                        : show help and exit" << endl
              << "  -v,--version                     : show version and exit" << endl
              << "  -c filename,--conf filename      : set configuration file (default: test.yaml)" << endl
              << "  -s signal,--signal signal        : send signal to a master process: stop, quit, reopen, reload" << endl;
}

void app::show_version_info() { std::cout << "zcp_tool version: 0.0.1" << endl; }

int app::signal_process() {
    std::string str_pid = load_file_data(pid_file_.c_str());
    if (str_pid.empty()) {
        LOG_ERROR("load file: {} data error", pid_file_);
        return -1;
    }
    pid_t pid = std::stoi(str_pid);
    for (const auto& signal : signals) {
        if (signal_cmd_ == signal.signal_cmd) {
            if (kill(pid, signal.sig_no) == -1) {
                LOG_SYSTEM_ERROR("kill pid: {}, signal: {}", pid, signal.sig_no);
                return -1;
            }
        }
    }
    return 0;
}

int app::init_signal() {
    for (const auto& signal : signals) {
        if (signal.sig_no != 0) {
            if (set_signal_handler(signal.sig_no, signal.handler) != 0) {
                LOG_ERROR("set signal handler error");
                return -1;
            }
        }
    }
    return 0;
}

int app::config_process() {
    YAML::Node root;
    if (yaml_load_file(config_file_, root) != 0) {
        LOG_ERROR("yaml load file: {} error", config_file_);
        return -1;
    }
    try {
        YAML::Node server_node = yaml_get_value<YAML::Node>(root, "server");
        config_.server.name = yaml_get_value<std::string>(server_node, "name");
        config_.server.process_num = yaml_get_value<uint8_t>(server_node, "process_num");
        config_.server.ip = yaml_get_value<std::string>(server_node, "ip");
        config_.server.port = yaml_get_value<uint16_t>(server_node, "port");
        config_.server.backlog = yaml_get_value<uint16_t>(server_node, "backlog");
        config_.server.event_num = yaml_get_value<uint16_t>(server_node, "event_num");
        YAML::Node log_node = yaml_get_value<YAML::Node>(root, "log");
        config_.log.name = yaml_get_value<std::string>(log_node, "name");
        config_.log.level = yaml_get_value<uint8_t>(log_node, "level");
        config_.log.file_name = yaml_get_value<std::string>(log_node, "file_name");
        config_.log.roll_type = yaml_get_value<std::string>(log_node, "roll_type");
        config_.log.reserve_count = yaml_get_value<unsigned int>(log_node, "reserve_count");
        config_.log.roll_size = yaml_get_value<unsigned int>(log_node, "roll_size");
        config_.log.async_thread_pool_size = yaml_get_value<unsigned int>(log_node, "async_thread_pool_size");
    } catch (const std::exception& ex) {
        LOG_ERROR("{}", ex.what());
        return -1;
    } catch (...) {
        LOG_ERROR("unknow exception");
        return -1;
    }
    LOG_DEBUG("name: {}, process_num: {}, ip: {}, port: {}, backlog: {}, event_num: {}", config_.server.name,
              config_.server.process_num, config_.server.ip, config_.server.port, config_.server.backlog,
              config_.server.event_num);
    return 0;
}

int app::daemon_process() {
    // const int daemon_change_dir = 0;
    const int daemon_unchange_dir = 1;
    // const int daemon_redirect_io = 0;
    const int daemon_un_redirect_io = 1;
    if (::daemon(daemon_unchange_dir, daemon_un_redirect_io) == -1) {
        LOG_SYSTEM_ERROR("daemon");
        return -1;
    }
    return 0;
}

int app::single_process() {
    single_process_ = open(lock_file_.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU);
    if (single_process_ == -1) {
        LOG_SYSTEM_ERROR("open");
        return -1;
    }
    if (single_process_.lock() != 0) {
        LOG_ERROR("lock fd: {} error", int(single_process_));
        return -1;
    }
    return 0;
}

int app::log_process() {
    logger_config config;
    config.name = config_.log.name;
    config.level = spdlog::level::level_enum(config_.log.level);
    config.file_name = config_.log.file_name;
    config.roll_type = config_.log.roll_type;
    config.reserve_count = config_.log.reserve_count;
    config.roll_size = config_.log.roll_size;
    config.async_thread_pool_size = config_.log.async_thread_pool_size;
    if (logger::get_instance()->set_config(config) != 0) {
        LOG_ERROR("set_config error");
        return -1;
    }
    return 0;
}

int app::create_pid_file() {
    fd_guard fd(open(pid_file_.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU));
    if (fd == -1) {
        LOG_SYSTEM_ERROR("open");
        return -1;
    }
    fd_lock_guard fd_lock(fd);
    if (fd_lock.lock(false) == -1) {
        LOG_ERROR("lock fd: {} error", int(fd_lock));
        return -1;
    }
    std::string data = std::to_string(getpid());
    if (write_file(fd, data.data(), data.size()) == -1) {
        LOG_ERROR("write file error");
        return -1;
    }
    return 0;
}

int app::fork_child() {
    pid_t pid = fork();
    if (pid == -1) {
        LOG_SYSTEM_ERROR("fork");
        return -1;
    } else if (pid == 0) {
        //子进程
        if (log_process() != 0) {
            LOG_ERROR("log process error");
            return -1;
        }
        server svr;
        int status = svr.start(config_.server.ip, config_.server.port, config_.server.backlog, config_.server.event_num);
        //退出，不然还会接着执行for循环创建更多的子进程的子进程
        _exit(status);
    }
    // pid > 0
    else {
        //父进程
        LOG_DEBUG("fork child pid: {}", pid);
    }
    return 0;
}

void app::check() {}