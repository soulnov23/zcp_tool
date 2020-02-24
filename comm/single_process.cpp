#include "single_process.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define PROCESS_SINGLE_SUCESS 0
#define PROCESS_BEEN_RUNUING -1
#define PROCESS_NAME_ERROR -2
#define CREATE_LOCKFILE_FAILURE -3

int single_process(const char* proc_name) {
    char file_lock_name[512] = "/var/run/single.";

    if (strlen(proc_name) >=
        (sizeof(file_lock_name) - strlen(file_lock_name))) {
        /*传入参数名字太长*/
        return PROCESS_NAME_ERROR;
    }

    strcat(file_lock_name, proc_name);

    int file = open(file_lock_name, O_WRONLY | O_CREAT,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == file) {
        /*创建文件失败*/
        return CREATE_LOCKFILE_FAILURE;
    }
    int flag = fcntl(file, F_GETFD);
    flag |= FD_CLOEXEC;
    fcntl(file, F_SETFD, flag);
    struct flock file_lock;
    file_lock.l_type = F_WRLCK;
    file_lock.l_start = 0;
    file_lock.l_whence = SEEK_SET;
    file_lock.l_len = 0;
    if (-1 == fcntl(file, F_SETLK, &file_lock)) {
        if (errno == EACCES || errno == EAGAIN) {
            /*该程序已经有一个进程实例在运行*/
            return PROCESS_BEEN_RUNUING;
        } else {
            return PROCESS_SINGLE_SUCESS;
        }
    }

    return PROCESS_SINGLE_SUCESS;
}
