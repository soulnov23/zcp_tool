#pragma once

#include <sys/socket.h>

#include <cstdint>
#include <string>
#include <string_view>

// 按 network 名字创建套接字，对应 netpoll 的 Socket。
// 支持的 network：tcp/tcp4/tcp6/udp/udp4/udp6/unix/unixgram/unixpacket/ip/ip4/ip6。
// 成功返回 fd，失败返回 -1。
// 建出来就是非阻塞 + CLOEXEC：SOCK_NONBLOCK/SOCK_CLOEXEC 在 socket() 里直接生效，
// 比建完之后再 fcntl 少两次系统调用，也没有"建好到设上标志之间被 fork 继承"的窗口
int create_socket(std::string_view network);

// 把 "ip:port" 或 unix socket 路径解析成 sockaddr，对应 netpoll 的 ResolveSockaddr。
// addr/addr_len 由调用方提供存储，成功返回 0 并回填长度。
// passive 为 true 时用于 bind（通配地址走 AI_PASSIVE），false 时用于 connect。
// getaddrinfo 是阻塞调用，只允许出现在启动和 accept/错误路径上
int resolve_sockaddr(std::string_view network, std::string_view address, struct sockaddr_storage* addr,
                     socklen_t* addr_len, bool passive);

// 把 sockaddr 格式化成 "ip:port"，对应 netpoll 的 SockaddrToAddr。
// 用 inet_ntop 而不是 inet_ntoa：后者返回静态缓冲区，同一条日志里调两次会互相覆盖
[[nodiscard]] std::string sockaddr_to_string(const struct sockaddr* addr, socklen_t addr_len);

// accept 并把新连接设成非阻塞 + CLOEXEC，对应 netpoll 调用方里的 unix.Accept4。
// 成功返回 fd，失败返回 -1，调用方按 errno 区分 EAGAIN/EINTR/其它
int accept_socket(int listen_fd, struct sockaddr_storage* addr, socklen_t* addr_len);

// 系统允许的监听队列上限，对应 netpoll 的 MaxListenerBacklog。
// 配置里的 backlog 超过这个值会被内核静默截断，所以调用方取两者较小值
[[nodiscard]] int max_listener_backlog();
