#include "printf_utils.h"
#include "xml_parser.h"
#include "server.h"
#include <unistd.h>

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
    PRINTF_DEBUG("ip:%s port:%s count:%s", config["ip"].c_str(),
                 config["port"].c_str(), config["count"].c_str());

    int count = strtol(config["count"].c_str(), nullptr, 10);
    
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
