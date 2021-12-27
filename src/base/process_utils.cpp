#include "src/base/process_utils.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <streambuf>

#include "src/base/fd_guard.h"
#include "src/base/fd_lock_guard.h"
#include "src/base/file_utils.h"
#include "src/base/printf_utils.h"

#define PROCESS_SINGLE_SUCESS   0
#define PROCESS_BEEN_RUNUING    -1
#define PROCESS_NAME_ERROR      -2
#define CREATE_LOCKFILE_FAILURE -3

int get_pid_file(const char* file_path) {
    std::string str_pid = file_to_string(file_path);
    if (str_pid.empty()) {
        PRINTF_DEBUG("file_to_string err");
        return -1;
    }
    return std::stoi(str_pid);
}

int set_pid_file(const char* file_path) {
    std::string str_pid = std::to_string(getpid());
    if (string_to_file(file_path, str_pid) == -1) {
        PRINTF_DEBUG("string_to_file err");
        unlink(file_path);
        return -1;
    }
    return 0;
}

int single_process(const char* proc_name) {
    char file_lock_name[64] = "/var/run/single.";
    strcat(file_lock_name, proc_name);
    fd_guard fd(open(file_lock_name, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU));
    if (fd == -1) {
        PRINTF_ERROR("open %s err", file_lock_name);
        return -1;
    }
    fd_lock_guard fd_lock(fd);
    return fd_lock.lock();
}

int daemon_process() {
    const int daemon_change_dir = 0;
    const int daemon_unchange_dir = 1;
    const int daemon_redirect_io = 0;
    const int daemon_un_redirect_io = 1;
    return daemon(daemon_unchange_dir, daemon_un_redirect_io);
}