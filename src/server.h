#ifndef __SERVER_H__
#define __SERVER_H__

#include <map>
#include <memory>
using namespace std;
#include "net/connector.h"
#include "net/net_utils.h"

class server {
public:
    server();
    ~server();

    int start();

private:
    void stop();

private:
    static void signal_handle_func(int sig_no, siginfo_t* sig_info, void* data);

private:
    int init_pid_file();
    int init_signal();
    int connect_timeout(int nsec, int usec);
    int do_listen();
    void do_accept();
    void do_recv(int fd);
    void do_send(int fd, const char* data, int len);
    void event_loop();
    void force_exit();

private:
    bool m_flag;
    int m_epoll_fd;
    int m_listen_fd;
    int m_client_fd;
    map<int, shared_ptr<connector>> m_fd_conn;
};

#endif