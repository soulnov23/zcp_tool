#include "src/base/process_util.h"

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
#include "src/base/file_util.h"
#include "src/base/log.h"

int single_process(const char* proc_name) {
    char file_lock_name[64] = "/var/run/single.";
    strcat(file_lock_name, proc_name);
    fd_guard fd(open(file_lock_name, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, S_IRWXU));
    if (fd == -1) {
        LOG_SYSTEM_ERROR("open file: {}", file_lock_name);
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