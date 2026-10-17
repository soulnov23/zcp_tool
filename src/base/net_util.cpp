#include "src/base/net_util.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "src/base/log.h"

namespace {

    // 置上一个布尔型的套接字选项。
    // 选项值统一走这里，避免在每个make_socket_xxx里重复写setsockopt时把参数传错
    int set_socket_bool_option(int fd, int level, int optname, const char* optname_str) {
        const int flag = 1;
        if (setsockopt(fd, level, optname, &flag, sizeof(flag)) == -1) {
            LOG_SYSTEM_ERROR("setsockopt fd: {}, option: {}", fd, optname_str);
            return -1;
        }
        return 0;
    }

    int set_socket_int_option(int fd, int level, int optname, int value, const char* optname_str) {
        if (setsockopt(fd, level, optname, &value, sizeof(value)) == -1) {
            LOG_SYSTEM_ERROR("setsockopt fd: {}, option: {}, value: {}", fd, optname_str, value);
            return -1;
        }
        return 0;
    }

    // 读出fd的标志位，置上add_flags、清掉del_flags之后写回。
    // get_cmd/set_cmd成对传F_GETFL/F_SETFL（文件状态标志）或者F_GETFD/F_SETFD（描述符标志），
    // 两组标志位的取值空间不同，不能混用
    int update_fd_flags(int fd, int get_cmd, int set_cmd, int add_flags, int del_flags) {
        // F_GETFL/F_GETFD不取第三个参数，不传多余的实参
        const int old_flags = fcntl(fd, get_cmd);
        if (old_flags == -1) {
            LOG_SYSTEM_ERROR("fcntl get flags fd: {}", fd);
            return -1;
        }
        const int new_flags = (old_flags | add_flags) & ~del_flags;
        // 标志位没有变化时省掉一次系统调用
        if (new_flags == old_flags) {
            return 0;
        }
        if (fcntl(fd, set_cmd, new_flags) == -1) {
            LOG_SYSTEM_ERROR("fcntl set flags fd: {}", fd);
            return -1;
        }
        return 0;
    }

}  // namespace

int make_socket_blocking(int fd) { return update_fd_flags(fd, F_GETFL, F_SETFL, 0, O_NONBLOCK); }

int make_socket_nonblocking(int fd) { return update_fd_flags(fd, F_GETFL, F_SETFL, O_NONBLOCK, 0); }

// FD_CLOEXEC是描述符标志，走F_GETFD/F_SETFD而不是F_GETFL/F_SETFL
int make_socket_cloexec(int fd) { return update_fd_flags(fd, F_GETFD, F_SETFD, FD_CLOEXEC, 0); }

int make_socket_reuseaddr(int fd) { return set_socket_bool_option(fd, SOL_SOCKET, SO_REUSEADDR, "SO_REUSEADDR"); }

int make_socket_reuseport(int fd) { return set_socket_bool_option(fd, SOL_SOCKET, SO_REUSEPORT, "SO_REUSEPORT"); }

// TCP_NODELAY在IPPROTO_TCP层，不在SOL_SOCKET层。
// 两个常量的值都是1，传错层不会报错而是变成了设置另一个选项（SO_DEBUG）
int make_socket_tcpnodelay(int fd) { return set_socket_bool_option(fd, IPPROTO_TCP, TCP_NODELAY, "TCP_NODELAY"); }

int make_socket_keepalive(int fd) { return set_socket_bool_option(fd, SOL_SOCKET, SO_KEEPALIVE, "SO_KEEPALIVE"); }

int set_socket_rcvbuf(int fd, int bufsize) { return set_socket_int_option(fd, SOL_SOCKET, SO_RCVBUF, bufsize, "SO_RCVBUF"); }

int set_socket_sndbuf(int fd, int bufsize) { return set_socket_int_option(fd, SOL_SOCKET, SO_SNDBUF, bufsize, "SO_SNDBUF"); }

int set_signal_handler(int sig_no, void (*handler)(int, siginfo_t*, void*), int flags) {
    struct sigaction action{};
    // sigset_t是不透明类型，清零不保证等于空信号集，必须用sigemptyset
    if (sigemptyset(&action.sa_mask) == -1) {
        LOG_SYSTEM_ERROR("sigemptyset");
        return -1;
    }
    // handler带siginfo_t参数，必须赋给sa_sigaction并置上SA_SIGINFO，
    // 否则内核按sa_handler的签名调用，多出来的两个参数是未定义的
    action.sa_flags = flags | SA_SIGINFO;
    action.sa_sigaction = handler;
    if (sigaction(sig_no, &action, nullptr) == -1) {
        LOG_SYSTEM_ERROR("sigaction sig: {}", sig_no);
        return -1;
    }
    return 0;
}
