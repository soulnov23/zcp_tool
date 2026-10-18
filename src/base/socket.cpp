#include "src/base/socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>

#include "src/base/eintr.h"
#include "src/base/log.h"

namespace {

    // 一个 network 名字拆成 socket() 需要的三个参数。
    // 返回 0 表示认识这个名字，-1 表示不支持
    int parse_network(std::string_view network, int* domain, int* type, int* protocol) {
        if (network == "tcp" || network == "tcp4") {
            *domain = AF_INET;
            *type = SOCK_STREAM;
            *protocol = IPPROTO_TCP;
        } else if (network == "tcp6") {
            *domain = AF_INET6;
            *type = SOCK_STREAM;
            *protocol = IPPROTO_TCP;
        } else if (network == "udp" || network == "udp4") {
            *domain = AF_INET;
            *type = SOCK_DGRAM;
            *protocol = IPPROTO_UDP;
        } else if (network == "udp6") {
            *domain = AF_INET6;
            *type = SOCK_DGRAM;
            *protocol = IPPROTO_UDP;
        } else if (network == "unix") {
            *domain = AF_UNIX;
            *type = SOCK_STREAM;
            *protocol = 0;
        } else if (network == "unixgram") {
            *domain = AF_UNIX;
            *type = SOCK_DGRAM;
            *protocol = 0;
        } else if (network == "unixpacket") {
            *domain = AF_UNIX;
            *type = SOCK_SEQPACKET;
            *protocol = 0;
        } else if (network == "ip" || network == "ip4") {
            *domain = AF_INET;
            *type = SOCK_RAW;
            *protocol = IPPROTO_IP;
        } else if (network == "ip6") {
            *domain = AF_INET6;
            *type = SOCK_RAW;
            *protocol = IPPROTO_IPV6;
        } else {
            return -1;
        }
        return 0;
    }

    bool is_unix(std::string_view network) {
        return network == "unix" || network == "unixgram" || network == "unixpacket";
    }

    // struct sockaddr_un 定长 108 字节的 sun_path 里只有前一段是有效路径。
    // 传 sizeof(sockaddr_un) 会把栈上的垃圾一起交给内核，绑定行为就成了未定义的
    socklen_t unix_addr_len(const struct sockaddr_un* addr) {
        const socklen_t head = static_cast<socklen_t>(offsetof(struct sockaddr_un, sun_path));
        const socklen_t name = static_cast<socklen_t>(strnlen(addr->sun_path, sizeof(addr->sun_path)));
        return static_cast<socklen_t>(head + name + 1);
    }

}  // namespace

int create_socket(std::string_view network) {
    int domain = 0;
    int type = 0;
    int protocol = 0;
    if (parse_network(network, &domain, &type, &protocol) != 0) {
        LOG_ERROR("network: {} not support", network);
        return -1;
    }
    const int fd = socket(domain, type | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
    if (fd == -1) {
        LOG_SYSTEM_ERROR("socket network: {}", network);
        return -1;
    }
    return fd;
}

int resolve_sockaddr(std::string_view network, std::string_view address, struct sockaddr_storage* addr,
                     socklen_t* addr_len, bool passive) {
    if (is_unix(network)) {
        struct sockaddr_un un {};
        un.sun_family = AF_UNIX;
        if (address.size() >= sizeof(un.sun_path)) {
            LOG_ERROR("unix address too long: {} size: {}", address, address.size());
            return -1;
        }
        std::memcpy(un.sun_path, address.data(), address.size());
        *addr_len = unix_addr_len(&un);
        std::memcpy(addr, &un, *addr_len);
        return 0;
    }

    // address 是 "ip:port" 形式。没有冒号时按纯地址处理，port 留空
    std::string host(address);
    std::string service;
    const auto pos = host.rfind(':');
    if (pos != std::string::npos) {
        service = host.substr(pos + 1);
        host.resize(pos);
    }

    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // 端口是纯数字，禁止 getaddrinfo 去查 /etc/services。
    // AI_PASSIVE 只在 bind 通配地址时加，connect 时加了会拿到 0.0.0.0
    hints.ai_flags = AI_NUMERICSERV | (passive ? AI_PASSIVE : 0);
    if (network == "tcp6" || network == "udp6" || network == "ip6") {
        hints.ai_family = AF_INET6;
    } else if (network == "tcp4" || network == "tcp" || network == "udp4" || network == "udp" || network == "ip" ||
               network == "ip4") {
        hints.ai_family = AF_INET;
    }

    struct addrinfo* result = nullptr;
    const int ret = getaddrinfo(host.empty() ? nullptr : host.c_str(), service.empty() ? nullptr : service.c_str(), &hints,
                                &result);
    if (ret != 0) {
        // EAI_AGAIN 是临时失败，一般是 DNS 瞬时问题，日志里单独点出来便于排查
        if (ret == EAI_AGAIN) {
            LOG_ERROR("getaddrinfo address: {} temporary failure", address);
        } else {
            LOG_ERROR("getaddrinfo address: {} error: {}", address, gai_strerror(ret));
        }
        return -1;
    }
    // 链表里第一个不一定能用（比如返回了 IPv6 但本机没开），逐个试
    int status = -1;
    for (struct addrinfo* it = result; it != nullptr; it = it->ai_next) {
        if (it->ai_addrlen > sizeof(struct sockaddr_storage)) {
            continue;
        }
        std::memcpy(addr, it->ai_addr, it->ai_addrlen);
        *addr_len = static_cast<socklen_t>(it->ai_addrlen);
        status = 0;
        break;
    }
    freeaddrinfo(result);
    if (status != 0) {
        LOG_ERROR("getaddrinfo address: {} no usable result", address);
    }
    return status;
}

std::string sockaddr_to_string(const struct sockaddr* addr, socklen_t addr_len) {
    if (addr == nullptr) {
        return std::string();
    }
    char host[NI_MAXHOST] = {0};
    char service[NI_MAXSERV] = {0};
    // NI_NUMERICHOST 只做格式化，不做反向解析：getnameinfo 在错误路径上被调用，
    // 不能因为它去查 DNS 把事件循环卡住
    if (getnameinfo(addr, addr_len, host, sizeof(host), service, sizeof(service),
                    NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
        return std::string();
    }
    std::string result(host);
    result += ':';
    result += service;
    return result;
}

int accept_socket(int listen_fd, struct sockaddr_storage* addr, socklen_t* addr_len) {
    // accept4 一次完成"接受 + 非阻塞 + CLOEXEC"，省掉 accept 之后的两次 fcntl。
    // 先清零 addr_len，内核会回填真实长度
    if (addr != nullptr && addr_len != nullptr) {
        *addr_len = static_cast<socklen_t>(sizeof(struct sockaddr_storage));
    }
    const int fd = handle_eintr([&] {
        return ::accept4(listen_fd, addr == nullptr ? nullptr : reinterpret_cast<struct sockaddr*>(addr), addr_len,
                         SOCK_NONBLOCK | SOCK_CLOEXEC);
    });
    return fd;
}

int max_listener_backlog() {
    // somaxconn 是 listen() 的实际上限，配置写得再大内核也只按它截。
    // 直接 open+read，不经过流式 IO：procfs 的 st_size 是 0，按大小预分配会读到空
    const int fd = ::open("/proc/sys/net/core/somaxconn", O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        LOG_SYSTEM_ERROR("open somaxconn");
        return SOMAXCONN;
    }
    char buffer[16] = {0};
    const ssize_t size = handle_eintr([&] { return ::read(fd, buffer, sizeof(buffer) - 1); });
    static_cast<void>(ignore_eintr(::close, fd));
    if (size <= 0) {
        LOG_ERROR("read somaxconn error, fallback to: {}", SOMAXCONN);
        return SOMAXCONN;
    }
    try {
        const int value = std::stoi(std::string(buffer, static_cast<size_t>(size)));
        // 和 netpoll 一样夹到 [1, 65535]：某些内核对越界值的处理不一致
        return std::clamp(value, 1, 65535);
    } catch (...) {
        LOG_ERROR("parse somaxconn error, fallback to: {}", SOMAXCONN);
        return SOMAXCONN;
    }
}
