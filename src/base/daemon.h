#ifndef __LIB_DAEMON_H__
#define __LIB_DAEMON_H__

/**
 * init_daemon - 将进程变成守护进程
 *
 * @param __nochdir 是否要切换当前工作目录到根目录
 * @param __noclose 是否要重定向标准输入输出错误到/dev/null
 *
 * @return 0 成功
 *        -1 fork失败
 *        -2 改变当前目录失败
 *
 * @note 返回失败时，标准输入输出错误不会关闭
 **/

int init_daemon(int __nochdir, int __noclose);

#endif
