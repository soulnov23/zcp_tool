#pragma once

#include <signal.h>

int make_socket_blocking(int fd);
int make_socket_nonblocking(int fd);
int make_socket_reuseaddr(int fd);
int make_socket_reuseport(int fd);
int make_socket_tcpnodelay(int fd);
int make_socket_keepalive(int fd);
int make_socket_cloexec(int fd);

int set_socket_rcvbuf(int fd, int bufsize);
int set_socket_sndbuf(int fd, int bufsize);

// 注册信号处理函数，成功返回0失败返回-1。
// flags直接传给sigaction的sa_flags，SA_SIGINFO由函数内部补上。
// 刻意不默认加SA_RESTART：本项目的策略是在调用点用eintr.h的handle_eintr处理EINTR，
// 需要让慢系统调用自动重启的话，由调用方显式传SA_RESTART
int set_signal_handler(int sig_no, void (*handler)(int, siginfo_t*, void*), int flags = 0);
