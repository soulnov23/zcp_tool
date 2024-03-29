#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "src/base/log.h"
#include "src/base/net_util.h"
#include "src/test/const.h"

shared_ptr<server> server::g_server = make_shared<server>();

server::server() {
    m_epoll_fd = -1;
    m_tcp_listen_fd = -1;
    m_udp_fd = -1;
    m_unix_fd = -1;
    m_raw_fd = -1;
    m_flag = true;
}

server::~server() { stop(); }

shared_ptr<server> server::get_instance() { return g_server; }

void server::signal_handler_t(int signum) {
    if (signum == SIGPIPE) {
        LOG_DEBUG("recv SIGPIPE signal");
    }
    if (signum == SIGINT) {
        LOG_DEBUG("recv SIGINT signal");
        g_server->stop();
    }
    if (signum == SIGTERM) {
        LOG_DEBUG("recv SIGTERM signal");
    }
}

int server::start() {
    m_epoll_fd = epoll_create1(0);
    if (-1 == m_epoll_fd) {
        LOG_ERROR("epoll_create1 error");
        return -1;
    }
    if (SIG_ERR == signal(SIGPIPE, signal_handler_t)) {
        LOG_ERROR("signal error");
        return -1;
    }
    if (SIG_ERR == signal(SIGINT, signal_handler_t)) {
        LOG_ERROR("signal error");
        return -1;
    }
    if (SIG_ERR == signal(SIGTERM, signal_handler_t)) {
        LOG_ERROR("signal error");
        return -1;
    }
    if (-1 == tcp_socket_start()) {
        LOG_ERROR("tcp_socket_start error");
        return -1;
    }
    if (-1 == udp_socket_start()) {
        LOG_ERROR("udp_socket_start error");
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
    if (m_tcp_listen_fd != -1) {
        if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_tcp_listen_fd, nullptr)) {
            LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, m_tcp_listen_fd);
        }
        close(m_tcp_listen_fd);
        m_tcp_listen_fd = -1;
    }
    if (m_udp_fd != -1) {
        if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_udp_fd, nullptr)) {
            LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, m_udp_fd);
        }
        close(m_udp_fd);
        m_udp_fd = -1;
    }
    if (m_unix_fd != -1) {
        if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_unix_fd, nullptr)) {
            LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, m_unix_fd);
        }
        close(m_unix_fd);
        m_unix_fd = -1;
    }
    if (m_raw_fd != -1) {
        if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_raw_fd, nullptr)) {
            LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, m_raw_fd);
        }
        close(m_raw_fd);
        m_raw_fd = -1;
    }
}

int server::tcp_socket_start() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_LISTEN_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    m_tcp_listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == make_socket_nonblocking(m_tcp_listen_fd)) {
        LOG_ERROR("make_socket_nonblocking({}) error", m_tcp_listen_fd);
        return -1;
    }
    struct epoll_event event;
    event.data.fd = m_tcp_listen_fd;
    event.events = EPOLLIN | EPOLLET;
    if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_tcp_listen_fd, &event)) {
        LOG_ERROR("epoll_ctl({}, EPOLL_CTL_ADD, {}) error", m_epoll_fd, m_tcp_listen_fd);
        return -1;
    }
    if (-1 == make_socket_reuseaddr(m_tcp_listen_fd)) {
        LOG_ERROR("make_socket_reuseaddr({}) error", m_tcp_listen_fd);
        return -1;
    }
    if (-1 == bind(m_tcp_listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        LOG_ERROR("bind({}, {}) error", m_tcp_listen_fd, inet_ntoa(server_addr.sin_addr));
        close(m_tcp_listen_fd);
        m_tcp_listen_fd = -1;
        return -1;
    }
    if (-1 == listen(m_tcp_listen_fd, 128)) {
        LOG_ERROR("listen({}, 128) error", m_tcp_listen_fd);
        close(m_tcp_listen_fd);
        m_tcp_listen_fd = -1;
        return -1;
    }
    return 0;
}

int server::udp_socket_start() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_LISTEN_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == make_socket_nonblocking(m_udp_fd)) {
        LOG_ERROR("make_socket_nonblocking({}) error", m_udp_fd);
        return -1;
    }
    struct epoll_event event;
    event.data.fd = m_udp_fd;
    event.events = EPOLLIN | EPOLLET;
    if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_udp_fd, &event)) {
        LOG_ERROR("epoll_ctl({}, EPOLL_CTL_ADD, {}) error", m_epoll_fd, m_udp_fd);
        return -1;
    }
    if (-1 == make_socket_reuseaddr(m_udp_fd)) {
        LOG_ERROR("make_socket_reuseaddr({}) error", m_udp_fd);
        return -1;
    }
    if (-1 == bind(m_udp_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        LOG_ERROR("bind({}, {}) error", m_udp_fd, inet_ntoa(server_addr.sin_addr));
        close(m_udp_fd);
        m_udp_fd = -1;
        return -1;
    }
    if (-1 == set_socket_rcvbuf(m_udp_fd, UDP_RCV_BUF)) {
        LOG_ERROR("set_socket_rcvbuf({}, {}) error", m_udp_fd, UDP_RCV_BUF);
        return -1;
    }
    if (-1 == set_socket_sndbuf(m_udp_fd, UDP_SND_BUF)) {
        LOG_ERROR("set_socket_sndbuf({}, {}) error", m_udp_fd, UDP_SND_BUF);
        return -1;
    }
    return 0;
}

void server::event_loop() {
    struct epoll_event events[128];
    while (m_flag) {
        int count = epoll_wait(m_epoll_fd, events, 128, -1);
        if (-1 == count) {
            LOG_ERROR("epoll_wait({}) error", m_epoll_fd);
            return;
        }
        for (int i = 0; i < count; i++) {
            if (events[i].events & EPOLLIN) {
                if (events[i].data.fd == m_tcp_listen_fd) {
                    do_tcp_accept();
                } else if (events[i].data.fd == m_udp_fd) {
                    do_udp_recvfrom();
                } else {
                    do_tcp_recv(events[i].data.fd);
                }
            } else if (events[i].events & EPOLLOUT) {
                LOG_DEBUG("EPOLLOUT");
            }
        }
    }
}

void server::do_tcp_accept() {
    while (true) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int fd = accept(m_tcp_listen_fd, (struct sockaddr*)&addr, &addr_len);
        if (fd == -1) {
            break;
        }
        LOG_DEBUG("(TCP)New accept ip:{} socket:{}", inet_ntoa(addr.sin_addr), fd);
        if (-1 == make_socket_nonblocking(fd)) {
            LOG_ERROR("make_socket_nonblocking({}) error", fd);
        }
        if (-1 == make_socket_tcpnodelay(fd)) {
            LOG_ERROR("make_socket_tcpnodelay({}) error", fd);
        }
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET;
        if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
            LOG_ERROR("epoll_ctl({}, EPOLL_CTL_ADD, {}) error", m_epoll_fd, fd);
        }
        auto conn = make_shared<connector>(fd, inet_ntoa(addr.sin_addr), nullptr);
        m_fd_conn.insert(make_pair(fd, conn));
    }
}

void server::do_tcp_recv(int fd) {
    auto it = m_fd_conn.find(fd);
    if (it == m_fd_conn.end()) {
        LOG_ERROR("m_fd_conn(map)没有发现连接对象的fd:{}", fd);
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
                LOG_ERROR("fd:{} ip:{} abnormal disconnection", fd, conn.get()->m_ip);
                if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, fd);
                }
                m_fd_conn.erase(it);
                break;
            }
        }
        if (ret == 0) {
            LOG_ERROR("fd:{} ip:{} normal disconnection", fd, conn.get()->m_ip);
            if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, fd);
            }
            m_fd_conn.erase(it);
            break;
        }
    }
}

void server::do_tcp_send(int fd, const char* data, int len) {
    auto it = m_fd_conn.find(fd);
    if (it == m_fd_conn.end()) {
        LOG_ERROR("m_fd_conn(map)没有发现连接对象的fd:{}", fd);
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
                LOG_ERROR("fd:{} ip:{} abnormal disconnection", fd, conn.get()->m_ip);
                if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, fd);
                }
                m_fd_conn.erase(it);
                break;
            }
        }
        if (ret == 0) {
            LOG_ERROR("fd:{} ip:{} normal disconnection", fd, conn.get()->m_ip);
            if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, fd);
            }
            m_fd_conn.erase(it);
            break;
        }
    }
}

void server::do_udp_recvfrom() {
    while (true) {
        char buf[UDP_RCV_BUF] = {0};
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        ssize_t ret = recvfrom(m_udp_fd, buf, UDP_RCV_BUF, 0, (struct sockaddr*)&addr, &addr_len);
        if (ret > 0) {
            LOG_DEBUG("fd:{} recvfrom:{} data:{}", m_udp_fd, inet_ntoa(addr.sin_addr), buf);
            continue;
        } else if (ret == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                LOG_ERROR("fd:{} abnormal disconnection", m_udp_fd);
                if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_udp_fd, nullptr)) {
                    LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, m_udp_fd);
                }
                close(m_udp_fd);
                m_udp_fd = -1;
                break;
            }
        }
        if (ret == 0) {
            LOG_ERROR("fd:{} normal disconnection", m_udp_fd);
            if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_udp_fd, nullptr)) {
                LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, m_udp_fd);
            }
            close(m_udp_fd);
            m_udp_fd = -1;
            break;
        }
    }
}

void server::do_udp_sendto(int fd, const char* data, int len, struct sockaddr_in addr) {
    int total_send = 0;
    socklen_t addr_len = sizeof(addr);
    while (total_send < len) {
        int ret = sendto(fd, data + total_send, len - total_send, 0, (struct sockaddr*)&addr, addr_len);
        if (ret > 0) {
            total_send += ret;
            continue;
        } else if (ret == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                LOG_ERROR("fd:{} abnormal disconnection", fd);
                if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                    LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, fd);
                }
                close(fd);
                fd = -1;
                break;
            }
        }
        if (ret == 0) {
            LOG_ERROR("fd:{} normal disconnection", fd);
            if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr)) {
                LOG_ERROR("epoll_ctl({}, EPOLL_CTL_DEL, {}) error", m_epoll_fd, fd);
            }
            close(fd);
            fd = -1;
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    server test;
    test.start();
    return 0;
}