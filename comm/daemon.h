#ifndef __LIB_DAEMON_H__
#define __LIB_DAEMON_H__

/**
 * init_daemon - 将进程变成守护进程
 *
 * @param path 要切换到的当前工作目录。参数为NULL是切换到/tmp
 *
 * @return 0 成功
 *        -1 fork失败
 *        -2 改变当前目录失败
 *
 * @note 返回失败时，标准输入输出不会关闭
 **/

int init_daemon(char* path);

#endif
