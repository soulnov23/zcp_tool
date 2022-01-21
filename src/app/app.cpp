#include "src/app/app.h"

#include <getopt.h>

#include "src/app/server.h"
#include "src/base/file_util.h"
#include "src/base/log.h"
#include "src/base/process_util.h"
#include "src/base/yaml_parser.h"

int app::get_option(int argc, char* argv[]) {
    while (true) {
        static struct option long_options[] = {
            {"help", no_argument, nullptr, 'h'},
            {"version", no_argument, nullptr, 'v'},
            {"conf", required_argument, nullptr, 'c'},
            {"signal", required_argument, nullptr, 's'},
            {0, 0, 0, 0},
        };
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
                CONSOLE_ERROR("invalid option: -{}", char(optopt));
                return -1;
            case 'h':
                show_help_ = true;
                break;
            case 'v':
                show_version_ = true;
                break;
            case 'c':
                config_file_path_ = optarg;
                break;
            case 's':
                signal_cmd_ = optarg;
                if (strcmp(signal_cmd_, "stop") != 0 || strcmp(signal_cmd_, "quit") != 0 || strcmp(signal_cmd_, "reopen") != 0 ||
                    strcmp(signal_cmd_, "reload") != 0) {
                    CONSOLE_ERROR("invalid signal: {}", *signal_cmd_);
                    return -1;
                }
                break;
            case ':':
                CONSOLE_ERROR("requires parameter: -{}", char(optopt));
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

int app::fork_child() {
    __pid_t pid = fork();
    if (pid == -1) {
        CONSOLE_ERROR("fork error");
        return -1;
    } else if (pid == 0) {
        //子进程
        server test;
        test.start();
        //退出，不然还会接着执行for循环创建更多的子进程的子进程
        _exit(0);
    }
    // pid > 0
    else {
        //父进程
        CONSOLE_DEBUG("fork child pid:{}", pid);
    }
    return 0;
}

int app::start(int argc, char* argv[]) {
#ifdef DEBUG
    CONSOLE_DEBUG("this is debug version");
#endif
    if (get_option(argc, argv) != 0) {
        CONSOLE_ERROR("get option error");
        return -1;
    }
    if (show_version_) {
        show_version_info();
    }
    if (show_help_) {
        show_help_info();
    }
    /*
    CONSOLE_DEBUG("ip:{} port:{} count:{}", config["ip"].c_str(), config["port"].c_str(), config["count"].c_str());

    int count = strtol(config["count"].c_str(), nullptr, 10);

    //读完配置再daemon，因为daemon要是改变了工作目录，那么相对路径的配置文件会找不到
    CONSOLE_DEBUG("daemonize begin");
    if (daemon_process() != 0) {
        CONSOLE_ERROR("daemonize failed");
        return -1;
    }

    for (int i = 0; i < count; i++) {
        if (fork_child() != 0) {
            CONSOLE_ERROR("fork_child error");
            return -1;
        }
    }
    while (true) {
        sleep(10);
    }
    */
    return 0;
}