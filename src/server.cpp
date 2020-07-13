#include "server.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include "printf_utils.h"

#define SIZE_1K (1024)
#define SIZE_1M (SIZE_1K) * (SIZE_1K)

#define SERVER_IP "172.16.1.103"
#define TCP_LISTEN_PORT 8765
#define UDP_LISTEN_PORT 5678
#define UDP_SND_BUF SIZE_1K
#define UDP_RCV_BUF SIZE_1K

server::server() {
    m_epoll_fd = -1;
    m_listen_fd = -1;
    m_client_fd = -1;
    m_flag = true;
}

server::~server() { stop(); }

void server::signal_handle_func(int sig_no, siginfo_t* sig_info, void* data) {
    if (sig_no == SIGCHLD) {
        PRINTF_DEBUG("recv signal:SIGCHLD");
        while (true) {
            int status = 0;
            __pid_t pid = waitpid(-1, &status, WNOHANG);
            //设置了WNOHANG没有子进程了
            if (pid == 0) {
                break;
            } else if (pid == -1) {
                //设置了WNOHANG没有子进程了
                if (errno == ECHILD) {
                    break;
                } else {
                    PRINTF_ERROR();
                    continue;
                }
            }
            //成功回收pid>0
            int result = WIFEXITED(status);
            if (result == 0) {
                //正常退出
                PRINTF_DEBUG("pid:%d normal exit return 0", pid);
            } else {
                //异常退出
                int return_result = WEXITSTATUS(status);
                PRINTF_DEBUG("pid:%d abnormal exit return %d", pid,
                             return_result);
            }
        }
    } else if (sig_no == SIGINT) {
        PRINTF_DEBUG("recv signal:SIGINT");
        _exit(0);;
    } else if (sig_no == SIGTERM) {
        PRINTF_DEBUG("recv signal:SIGTERM");
        _exit(0);;
    } else if (sig_no == SIGQUIT) {
        PRINTF_DEBUG("recv signal:SIGQUIT");
        _exit(0);;
    } else if (sig_no == SIGUSR1) {
        PRINTF_DEBUG("recv signal:SIGUSR1");
    } else if (sig_no == SIGIO) {
        PRINTF_DEBUG("recv signal:SIGIO");
    } else if (sig_no == SIGPIPE) {
        PRINTF_DEBUG("recv signal:SIGPIPE");
    } else {
        PRINTF_DEBUG("no set signal:%d", sig_no);
    }
}

int server::start() {
    m_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epoll_fd == -1) {
        PRINTF_ERROR("epoll_create1 error");
        return -1;
    }
    if (init_signal() == -1) {
        PRINTF_ERROR("init_signal error");
        return -1;
    }
    if (do_listen() == -1) {
        PRINTF_ERROR("do_listen error");
        return -1;
    }
    event_loop();
    return 0;
}

void server::stop() {
    m_flag = false;
    if (m_epoll_fd != -1) {
        close(m_epoll_fd);
        m_epoll_fd = -1;
    }
    if (m_listen_fd != -1) {
        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_listen_fd, nullptr) == -1) {
            PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd,
                         m_listen_fd);
        }
        close(m_listen_fd);
        m_listen_fd = -1;
    }
    if (m_client_fd != -1) {
        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_client_fd, nullptr) == -1) {
            PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd,
                         m_client_fd);
        }
        close(m_client_fd);
        m_client_fd = -1;
    }
}

int server::init_pid_file() {
    int fd = open("server.pid", O_RDWR | O_CREAT | O_TRUNC | O_EXCL, 0644);
    if (fd < 0) {
        PRINTF_ERROR("open pid file failed");
        return -1;
    }

    pid_t pid = getpid();
    string str_pid = std::to_string(pid);
    int ret = -1;
    while ((ret = write(fd, str_pid.data(), str_pid.size()) == -1) &&
           errno == EINTR)
        ;
    if (ret == -1) {
        PRINTF_ERROR("write pid file failed");
        unlink("server.pid");
    } else {
        ret = 0;
    }

    close(fd);
    return ret;
}

int server::init_signal() {
    static const int sigs[] = {SIGINT,  SIGTERM, SIGQUIT, SIGCHLD,
                               SIGUSR1, SIGIO,   SIGPIPE};
    const int* begin = std::begin(sigs);
    const int* end = std::end(sigs);
    for (; begin != end; begin++) {
        int ret = set_signal_handle(*begin, signal_handle_func);
        if (ret != 0) {
            PRINTF_ERROR("set_signal_handle error");
            return -1;
        }
    }
    return 0;
}

int server::connect_timeout(int nsec, int usec) {
    int count;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_LISTEN_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    m_client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (m_client_fd == -1) {
        PRINTF_ERROR("socket error");
        return -1;
    }
    int ret = connect(m_client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == 0) {
        PRINTF_DEBUG("connect直接成功");
        return 0;
    }
    // ret=-1的情况
    if (errno != EINPROGRESS) {
        PRINTF_ERROR("connect error");
        goto error;
    }
    struct timeval timeout;
    timeout.tv_sec = nsec;
    timeout.tv_usec = usec;
    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(m_client_fd, &write_set);
    count = select(m_client_fd + 1, nullptr, &write_set, nullptr, &timeout);
    if (count == 0) {
        PRINTF_ERROR("select timeout");
        goto error;
    }
    if (count == -1) {
        PRINTF_ERROR("select error");
        goto error;
    }
    // count>0的情况
    if (FD_ISSET(m_client_fd, &write_set)) {
        int err = 0;
        socklen_t len = sizeof(err);
        if (getsockopt(m_client_fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1) {
            PRINTF_ERROR("getsockopt error");
            goto error;
        }
        if (err != 0) {
            PRINTF_ERROR("connect error");
            goto error;
        }
        return 0;
    } else {
        PRINTF_ERROR("FD_ISSET error");
        goto error;
    }

error:
    close(m_client_fd);
    m_client_fd = -1;
    return -1;
}

int server::do_listen() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_LISTEN_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    m_listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listen_fd == -1) {
        PRINTF_ERROR("socket error");
        return -1;
    }
    if (make_socket_nonblocking(m_listen_fd) == -1) {
        PRINTF_ERROR("make_socket_nonblocking(%d) error", m_listen_fd);
        goto error;
    }
    if (make_socket_cloexec(m_listen_fd) == -1) {
        PRINTF_ERROR("make_socket_cloexec(%d) error", m_listen_fd);
        goto error;
    }
    if (make_socket_reuseaddr(m_listen_fd) == -1) {
        PRINTF_ERROR("make_socket_reuseaddr(%d) error", m_listen_fd);
        goto error;
    }
    if (make_socket_reuseport(m_listen_fd) == -1) {
        PRINTF_ERROR("make_socket_reuseport(%d) error", m_listen_fd);
        goto error;
    }
    struct epoll_event event;
    event.data.fd = m_listen_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &event) == -1) {
        PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_ADD, %d) error", m_epoll_fd,
                     m_listen_fd);
        goto error;
    }
    if (bind(m_listen_fd, (struct sockaddr*)&server_addr,
                   sizeof(server_addr)) == -1) {
        PRINTF_ERROR("bind(%d, %s) error", m_listen_fd,
                     inet_ntoa(server_addr.sin_addr));
        goto error;
    }
    if (listen(m_listen_fd, 128) == -1) {
        PRINTF_ERROR("listen(%d, 128) error", m_listen_fd);
        goto error;
    }
    return 0;

error:
    close(m_listen_fd);
    m_listen_fd = -1;
    return -1;
}

void server::event_loop() {
    struct epoll_event events[128];
    while (m_flag) {
        int count = epoll_wait(m_epoll_fd, events, 128, -1);
        if (-1 == count) {
            PRINTF_ERROR("epoll_wait(%d) error", m_epoll_fd);
            return;
        }
        for (int i = 0; i < count; i++) {
            if (events[i].events & EPOLLIN) {
                if (events[i].data.fd == m_listen_fd) {
                    do_accept();
                } else {
                    do_recv(events[i].data.fd);
                }
            } else if (events[i].events & EPOLLOUT) {
                do_send(events[i].data.fd, "hello world", 11);
            }
        }
    }
}

void server::force_exit() {
    PRINTF_DEBUG("now force exit, killall -9(SIGKILL)");
    if (kill(0, SIGKILL) != 0) {
        PRINTF_ERROR("kill error")
    }
}

void server::do_accept() {
    while (true) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int fd = accept(m_listen_fd, (struct sockaddr*)&addr, &addr_len);
        if (fd == -1) {
            if (errno != EAGAIN)
            break;
        }
        PRINTF_DEBUG("(TCP)New accept ip:%s socket:%d",
                     inet_ntoa(addr.sin_addr), fd);
        if (-1 == make_socket_nonblocking(fd)) {
            PRINTF_ERROR("make_socket_nonblocking(%d) error", fd);
        }
        if (-1 == make_socket_cloexec(fd)) {
            PRINTF_ERROR("make_socket_cloexec(%d) error", fd);
        }
        if (-1 == make_socket_tcpnodelay(fd)) {
            PRINTF_ERROR("make_socket_tcpnodelay(%d) error", fd);
        }
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET;
        if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
            PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_ADD, %d) error", m_epoll_fd,
                         fd);
        }
        auto conn =
            make_shared<connector>(fd, inet_ntoa(addr.sin_addr), nullptr);
        m_fd_conn.insert(make_pair(fd, conn));
    }
}

void server::do_recv(int fd) {
    auto it = m_fd_conn.find(fd);
    if (it == m_fd_conn.end()) {
        PRINTF_ERROR("m_fd_conn(map)没有发现连接对象的fd:%d", fd);
        return;
    }
    auto conn = it->second;
    while (true) {
        char buf[1024] = {0};
        ssize_t ret = recv(fd, buf, 1024, 0);
        if (ret > 0) {
            conn.get()->m_buffer.get()->append(buf, ret);
            continue;
        } else if (ret == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                PRINTF_ERROR("fd:%d ip:%s abnormal disconnection", fd,
                             conn.get()->m_ip);
                if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error",
                                 m_epoll_fd, fd);
                }
                m_fd_conn.erase(it);
                break;
            }
        }
        if (ret == 0) {
            PRINTF_ERROR("fd:%d ip:%s normal disconnection", fd,
                         conn.get()->m_ip);
            if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error",
                             m_epoll_fd, fd);
            }
            m_fd_conn.erase(it);
            break;
        }
    }
}

void server::do_send(int fd, const char* data, int len) {
    auto it = m_fd_conn.find(fd);
    if (it == m_fd_conn.end()) {
        PRINTF_ERROR("m_fd_conn(map)没有发现连接对象的fd:%d", fd);
        return;
    }
    auto conn = it->second;
    int total_send = 0;
    while (total_send < len) {
        int ret = send(fd, data + total_send, len - total_send, 0);
        if (ret > 0) {
            total_send += ret;
            continue;
        } else if (ret == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                PRINTF_ERROR("fd:%d ip:%s abnormal disconnection", fd,
                             conn.get()->m_ip);
                if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error",
                                 m_epoll_fd, fd);
                }
                m_fd_conn.erase(it);
                break;
            }
        }
    }
}