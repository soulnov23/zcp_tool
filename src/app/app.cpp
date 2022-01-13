#include "src/app/app.h"

#include "src/app/server.h"
#include "src/base/file_util.h"
#include "src/base/log.h"
#include "src/base/process_util.h"
#include "src/base/yaml_parser.h"

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
    CONSOLE_DEBUG("{} {}", argv[0], argv[1]);
    if (argc < 2) {
        CONSOLE_ERROR("Usage: {} conf_file", argv[0]);
        CONSOLE_ERROR("Sample: {} ../conf/test.conf.xml", argv[0]);
        return -1;
    }

    const char* file_path = argv[1];
    if (read_config(file_path) != 0) {
        CONSOLE_ERROR("read_config error");
        return -1;
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

int app::read_config(const char* file_path) {
    YAML::Node root;
    if (yaml_load_file(root, file_path) != 0) {
        CONSOLE_ERROR("load_file err");
        return -1;
    }
    YAML::Node server = root["server"];
    if (!yaml_get_value<std::string>(root, "name", config_.server.name)) {
        CONSOLE_ERROR("get_value err");
        return -1;
    }
    return 0;
}