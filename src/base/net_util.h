#pragma once

#include <signal.h>

// 全部返回0成功、-1失败。错误码被忽略的后果是静默的（比如CLOEXEC没开成功，
// fork/exec后子进程会继承连接），所以这里统一标上nodiscard
[[nodiscard]] int make_socket_blocking(int fd);
[[nodiscard]] int make_socket_nonblocking(int fd);
[[nodiscard]] int make_socket_reuseaddr(int fd);
[[nodiscard]] int make_socket_reuseport(int fd);
[[nodiscard]] int make_socket_tcpnodelay(int fd);
[[nodiscard]] int make_socket_cloexec(int fd);

// 打开SO_KEEPALIVE并按给定的探测参数配置，对应 netpoll 的 SetSocketKeepAlive。
// cnt 探测次数、intvl 探测间隔（秒）、idle 空闲阈值（秒），
// 在内核里分别对应 TCP_KEEPCNT / TCP_KEEPINTVL / TCP_KEEPIDLE。
// 探测参数非要不可，只开SO_KEEPALIVE要等内核默认的7200秒才发第一个探测包，
// 对已经断掉的连接等于没检测
[[nodiscard]] int make_socket_keepalive(int fd, int cnt, int intvl, int idle);

[[nodiscard]] int set_socket_rcvbuf(int fd, int bufsize);
[[nodiscard]] int set_socket_sndbuf(int fd, int bufsize);

// 注册信号处理函数，成功返回0失败返回-1。
// flags直接传给sigaction的sa_flags，SA_SIGINFO由函数内部补上。
// 刻意不默认加SA_RESTART：本项目的策略是在调用点用eintr.h的handle_eintr处理EINTR，
// 需要让慢系统调用自动重启的话，由调用方显式传SA_RESTART
[[nodiscard]] int set_signal_handler(int sig_no, void (*handler)(int, siginfo_t*, void*), int flags = 0);
