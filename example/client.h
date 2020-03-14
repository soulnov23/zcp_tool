#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "net/connector.h"
#include <memory>
using namespace std;

class client {
   public:
    client();
    ~client();

    int start();

   private:
    void stop();

   public:
    static shared_ptr<client> get_instance();

   private:
    static void signal_handler_t(int signum);

   private:
    void do_tcp_recv();
    void do_tcp_send(int fd, const char* data, int len);
    void do_udp_recvfrom();
    void do_udp_sendto(int fd, const char* data, int len,
                       struct sockaddr_in addr);
    int tcp_socket_start();
    int connect_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen,
                        int nsec, int usec);
    int udp_socket_start();
    int unix_socket_start();
    int raw_socket_start();
    void event_loop();

   private:
    bool m_flag;
    int m_epoll_fd;
    int m_tcp_fd;
    shared_ptr<connector> m_tcp_conn;
    int m_udp_fd;
    int m_unix_fd;
    int m_raw_fd;
    static shared_ptr<client> g_client;
};

#endif