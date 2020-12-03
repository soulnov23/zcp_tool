#include "zcp_tool/comm/daemon.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int init_daemon(int __nochdir, int __noclose) {
    int pid;
    if ((pid = fork()) > 0) {
        exit(0); /* 是父进程，结束父进程 */
    } else if (pid < 0) {
        return -1; /* fork失败 */
    }
    /* 是第一子进程，继续 */
    /* 如果该进程是一个进程组的组长，此函数返回错误。为了保证这一点，我们先调用fork()然后exit()，此时只有子进程在运行，子进程继承了父进程的进程组ID，
    但是进程PID却是新分配的，所以不可能是新会话的进程组的PID。从而保证了这一点。 */
    setsid();
    /* 创建一个新的会话 */
    /* 第一子进程成为新的会话组长和进程组长 */
    /* 脱离控制终端，登录会话和进程组 */

    /* 现在，第一子进程已经成为无终端的会话组长。但它可以重新申请打开一个控制终端。可以通过使进程不再成为会话组长来禁止进程重新打开控制终端*/
    if ((pid = fork()) > 0) {
        exit(0); /* 是第一子进程，结束第一子进程 */
    } else if (pid < 0) {
        return -1; /* fork失败 */
    }

    /* 是第二子进程，继续 */
    /* 第二子进程不再是会话组长 */

    /* 改变工作目录到根目录 */
    /* 最好是切换到根目录，因为如果它的当前目录是在一个被安装的文件系统上,那么就会妨碍这个文件系统被卸载，非必须,防止mount出错 */
    if (__nochdir == 0) {
        if (chdir("/") != 0) {
            return -2;
        }
    }

    /* parasoft suppress item BD-RES-INVFREE-1 */
    /* 关闭标准输入输出，标准错误的文件描述符 */
    if (__noclose == 0) {
        close(STDIN_FILENO);  /* parasoft-suppress BD-RES-INVFREE-1 */
        close(STDOUT_FILENO); /* parasoft-suppress BD-RES-INVFREE-1 */
        close(STDERR_FILENO); /* parasoft-suppress BD-RES-INVFREE-1 */
    }
    /* parasoft unsuppress item BD-RES-INVFREE-1 */

    umask(0);
    /* 进程从创建它的父进程那里继承了文件创建掩模。它可能修改守护进程所创建的文件
    的存取位，umask调用把守护进程的umask设置为0，这样取消了父进程的umask，避免了潜在的
    干扰创建文件和目录 */

    return 0;
}
