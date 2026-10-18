#include "src/app/app.h"

#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "src/app/server.h"
#include "src/base/eintr.h"
#include "src/base/log.h"
#include "src/base/net_util.h"
#include "yaml-cpp/yaml.h"

namespace {

    // 配置文件路径。原来的命令行解析已经去掉，这里固定下来
    constexpr const char* config_file = "../conf/server.yaml";

    // master 收到的信号只记在这里，真正的转发在主循环里做。
    // sig_atomic_t 是标准保证可以和信号处理函数共享的类型
    volatile sig_atomic_t g_master_signal = 0;

    // worker 的唤醒 fd，信号处理函数里写它来唤醒 epoll
    volatile sig_atomic_t g_worker_wake_fd = -1;

    // 已完成初始化的 worker 进程号。master 只转发给这些进程
    std::vector<pid_t> g_workers;

    // 收到的是退出信号（SIGTERM/SIGQUIT）。转发之后等子进程全部回收完，master 自己也退出
    bool g_master_exit = false;

}  // namespace

void app::master_signal_handler(int sig_no, siginfo_t* sig_info, void* data) {
    // 信号处理函数里只做 async-signal-safe 的操作：不碰日志（spdlog 内部有锁，
    // 信号打断一次持锁的日志调用就是死锁），不碰堆，也不碰非原子的共享状态。
    // errno 要先存后恢复，否则被打断的系统调用会拿到假的 errno 判断
    const int saved_errno = errno;
    static_cast<void>(sig_info);
    static_cast<void>(data);
    g_master_signal = sig_no;
    errno = saved_errno;
}

void app::worker_signal_handler(int sig_no, siginfo_t* sig_info, void* data) {
    const int saved_errno = errno;
    static_cast<void>(sig_info);
    static_cast<void>(data);
    if (sig_no == SIGTERM || sig_no == SIGQUIT) {
        const int fd = g_worker_wake_fd;
        if (fd != -1) {
            uint64_t value = 1;
            static_cast<void>(::write(fd, &value, sizeof(value)));
        }
    }
    // SIGHUP/SIGUSR1/SIGUSR2 暂时没有对应动作，装上手是为了不让默认动作杀进程。
    // SIGPIPE 由 signal(SIGPIPE, SIG_IGN) 单独处理，不会进到这里
    errno = saved_errno;
}

app::app() : config_file_(config_file) {}

app::~app() = default;

int app::start(int argc, char* argv[]) {
    static_cast<void>(argc);
    static_cast<void>(argv);
    if (config_process(config_file_) != 0) {
        LOG_ERROR("config process error");
        return -1;
    }
    if (daemon_process() != 0) {
        LOG_ERROR("daemon process error");
        return -1;
    }
    // 日志配置要在 fork 之前应用：master 自己也要按配置输出，
    // 而且 fork 出来的 worker 直接继承同一个 sink，不需要各自再建一份文件。
    // 放在 daemon() 之后是因为重定向要等进程脱离终端后再做
    if (logger::instance().set_config(config_.log) != 0) {
        LOG_ERROR("set config error");
        return -1;
    }
    if (init_master_signal() != 0) {
        LOG_ERROR("init master signal error");
        return -1;
    }
    // 至少创建一个 worker
    const int process_num = config_.server.process_num > 0 ? config_.server.process_num : 1;
    for (int i = 0; i < process_num; i++) {
        if (fork_worker() != 0) {
            LOG_ERROR("fork worker error");
            return -1;
        }
    }
    run();
    return 0;
}

int app::config_process(const std::string& file) {
    try {
        // 用const Node：缺失的key会得到一个无效节点，取值时抛InvalidNode并带上key名，
        // 非const的operator[]则会往树里插一个未定义节点，报错也只剩下bad conversion
        const YAML::Node root = YAML::LoadFile(file);
        const YAML::Node server_node = root["server"];
        if (!server_node) {
            LOG_ERROR("config: {} has no server section", file);
            return -1;
        }
        config_.server.name = server_node["name"].as<std::string>();
        config_.server.process_num = server_node["process_num"].as<uint8_t>();
        config_.server.ip = server_node["ip"].as<std::string>();
        config_.server.port = server_node["port"].as<uint16_t>();
        config_.server.backlog = server_node["backlog"].as<uint16_t>();
        config_.server.event_num = server_node["event_num"].as<uint16_t>();
        const YAML::Node log_node = root["log"];
        if (!log_node) {
            LOG_ERROR("config: {} has no log section", file);
            return -1;
        }
        if (parse_log_config(log_node) != 0) {
            return -1;
        }
    } catch (const std::exception& ex) {
        LOG_ERROR("parse config: {} error: {}", file, ex.what());
        return -1;
    } catch (...) {
        LOG_ERROR("parse config: {} unknow exception", file);
        return -1;
    }
    LOG_DEBUG("name: {}, process_num: {}, ip: {}, port: {}, backlog: {}, event_num: {}", config_.server.name,
              config_.server.process_num, config_.server.ip, config_.server.port, config_.server.backlog,
              config_.server.event_num);
    return 0;
}

// level用名字配置，其余键都可以不写，缺省值见logger_config
// as<T>(fallback)在key缺失、值为空、类型对不上这三种情况下都返回缺省值，不抛异常
int app::parse_log_config(const YAML::Node& log_node) {
    logger_config config;
    config.name = log_node["name"].as<std::string>(config.name);
    config.format = log_node["format"].as<std::string>(config.format);
    config.file_name = log_node["file_name"].as<std::string>(config.file_name);
    const std::string level = log_node["level"].as<std::string>(std::string("trace"));
    // spdlog的级别名：trace debug info warning error critical off，
    // 认不出来时返回off，这里当配置写错处理
    config.level = spdlog::level::from_str(level);
    if (config.level == spdlog::level::off && level != "off") {
        LOG_ERROR("invalid log level: {}", level);
        return -1;
    }
    config_.log = std::move(config);
    return 0;
}

int app::daemon_process() {
    // 不切换工作目录：配置文件里的日志路径是相对 conf 目录写的
    const int daemon_unchange_dir = 1;
    const int daemon_un_redirect_io = 1;
    if (::daemon(daemon_unchange_dir, daemon_un_redirect_io) == -1) {
        LOG_SYSTEM_ERROR("daemon");
        return -1;
    }
    return 0;
}

int app::init_master_signal() {
    // SIGPIPE 直接忽略：对端断开后 write 返回 EPIPE，由 errno 分支处理
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        LOG_SYSTEM_ERROR("signal SIGPIPE");
        return -1;
    }
    // 退出/运维信号装上 handler，主循环里再转发给 worker
    constexpr int forward_signals[] = {SIGTERM, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2};
    for (const int sig_no : forward_signals) {
        if (set_signal_handler(sig_no, master_signal_handler) != 0) {
            LOG_ERROR("set master signal handler error sig: {}", sig_no);
            return -1;
        }
    }
    // SIGCHLD 单独处理：回收退出的 worker
    if (set_signal_handler(SIGCHLD, master_signal_handler) != 0) {
        LOG_ERROR("set master signal handler error sig: SIGCHLD");
        return -1;
    }
    return 0;
}

int app::init_worker_signal() {
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        LOG_SYSTEM_ERROR("signal SIGPIPE");
        return -1;
    }
    // SIGHUP/SIGUSR1/SIGUSR2 暂时没有动作，也要装上手，
    // 否则默认动作是终止进程，一个运维指令就能把 worker 打死
    constexpr int worker_signals[] = {SIGTERM, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2};
    for (const int sig_no : worker_signals) {
        if (set_signal_handler(sig_no, worker_signal_handler) != 0) {
            LOG_ERROR("set worker signal handler error sig: {}", sig_no);
            return -1;
        }
    }
    return 0;
}

int app::fork_worker() {
    const pid_t pid = fork();
    if (pid == -1) {
        LOG_SYSTEM_ERROR("fork");
        return -1;
    }
    if (pid == 0) {
        // 子进程：日志 sink 由 master 在 fork 前建好，这里只需要换掉从 master
        // 继承来的信号处理函数
        if (init_worker_signal() != 0) {
            LOG_ERROR("init worker signal error");
            _exit(-1);
        }
        server svr;
        svr.set_wake_fd_target(&g_worker_wake_fd);
        const int status =
            svr.start(config_.server.ip, config_.server.port, config_.server.backlog, config_.server.event_num);
        // 必须用 _exit：worker 继承了 master 的文件描述符，走 exit 会跑 atexit 链。
        // 日志是 flush_on(trace) 逐条刷的，不需要额外 flush
        _exit(status);
    }
    g_workers.push_back(pid);
    LOG_DEBUG("fork worker pid: {}", pid);
    return 0;
}

void app::run() {
    // 主循环只在收到信号时醒一次。用 sigsuspend 而不是忙轮询：
    // 它在等待期间把信号掩码放开，收到信号后处理函数跑完再返回，然后把掩码恢复成屏蔽态，
    // 这样标志位在一次休眠里不会被改两次
    sigset_t mask;
    sigset_t old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    // SIGCHLD 也要进 mask：sigsuspend 只会在等待期间收到"被屏蔽的信号"时返回，
    // 没屏蔽的信号会直接运行 handler 而不会唤醒这里
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    while (true) {
        // sigsuspend 在等待期间把掩码放开，收到信号后 handler 跑完再返回，
        // 返回时掩码又恢复成屏蔽态。
        // 返回 -1 + EINTR 是它被信号唤醒的唯一方式，这里要跳出循环去看标志位，
        // 不能像处理普通系统调用那样重试——重试会把已经收到的信号一直攒在标志位里没人处理，
        // 最终表现为信号风暴时停不下来
        sigsuspend(&old_mask);
        const int sig_no = static_cast<int>(g_master_signal);
        g_master_signal = 0;
        if (sig_no == 0) {
            continue;
        }
        if (sig_no == SIGCHLD) {
            reap_worker();
            // 退出信号已经转发出去，子进程全收完就轮到自己退出了
            if (g_master_exit && g_workers.empty()) {
                LOG_DEBUG("all worker exited, master exit");
                return;
            }
            continue;
        }
        if (sig_no == SIGTERM || sig_no == SIGQUIT) {
            g_master_exit = true;
            // 没有子进程时没人会再送来 SIGCHLD，这里直接退出
            if (g_workers.empty()) {
                LOG_DEBUG("no worker, master exit");
                return;
            }
        }
        forward_signal(sig_no);
    }
}

void app::forward_signal(int sig_no) {
    // 逐个 kill 而不是走进程组：master 只转自己 fork 出来的这几个进程
    for (const pid_t pid : g_workers) {
        if (::kill(pid, sig_no) == -1) {
            LOG_SYSTEM_ERROR("kill worker pid: {} signal: {}", pid, sig_no);
        }
    }
    LOG_DEBUG("forward signal: {} to {} worker(s)", sig_no, g_workers.size());
}

void app::reap_worker() {
    while (true) {
        int status = 0;
        const pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid == 0) {
            return;
        }
        if (pid == -1) {
            if (errno == EINTR) {
                continue;
            }
            // ECHILD 说明没有子进程了，正常
            if (errno != ECHILD) {
                LOG_SYSTEM_ERROR("waitpid");
            }
            return;
        }
        LOG_DEBUG("worker pid: {} exit status: {}", pid, status);
        // 从转发列表里摘掉，避免把信号发给已经回收、进程号可能被复用的进程
        for (auto it = g_workers.begin(); it != g_workers.end(); ++it) {
            if (*it == pid) {
                g_workers.erase(it);
                break;
            }
        }
    }
}
