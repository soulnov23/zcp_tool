#include <unistd.h>
#include "daemon.h"
#include "printf_utils.h"
#include "server.h"
#include "xml_parser.h"

map<string, string> config;

int fork_child() {
    __pid_t pid = fork();
    if (pid == -1) {
        PRINTF_ERROR("fork error");
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
        PRINTF_DEBUG("fork child pid:%d", pid);
    }
    return 0;
}

int main(int argc, char* argv[]) {
#ifdef DEBUG
    PRINTF_DEBUG("this is debug version");
#endif
    PRINTF_DEBUG("%s %s", argv[0], argv[1]);
    if (argc < 2) {
        PRINTF_ERROR("Usage: %s conf_file", argv[0]);
        PRINTF_ERROR("Sample: %s ../conf/test.conf.xml", argv[0]);
        return -1;
    }

    const char* file_path = argv[1];
    if (xml_to_map(config, file_path) != 0) {
        PRINTF_ERROR("xml_to_map error");
        return -1;
    }
    PRINTF_DEBUG("ip:%s port:%s count:%s", config["ip"].c_str(), config["port"].c_str(), config["count"].c_str());

    int count = strtol(config["count"].c_str(), nullptr, 10);

    //读完配置再daemon，因为daemon要是改变了工作目录，那么相对路径的配置文件会找不到
    PRINTF_DEBUG("daemonize begin");
    const int daemon_change_dir     = 0;
    const int daemon_unchange_dir   = 1;
    const int daemon_redirect_io    = 0;
    const int daemon_un_redirect_io = 1;
    if (daemon(daemon_unchange_dir, daemon_un_redirect_io) != 0) {
        PRINTF_ERROR("daemonize failed");
        return -1;
    }

    for (int i = 0; i < count; i++) {
        if (fork_child() != 0) {
            PRINTF_ERROR("fork_child error");
            return -1;
        }
    }
    while (true) {
        sleep(10);
    }
    return 0;
}
