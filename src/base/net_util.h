#ifndef __NET_UTILS_H__
#define __NET_UTILS_H__

#include <string>
using namespace std;
#include <signal.h>
#include <unistd.h>

int make_socket_blocking(int fd);
int make_socket_nonblocking(int fd);
int make_socket_reuseaddr(int fd);
int make_socket_reuseport(int fd);
int make_socket_tcpnodelay(int fd);
int make_socket_keepalive(int fd);
int make_socket_cloexec(int fd);

int set_socket_rcvbuf(int fd, int bufsize);
int set_socket_sndbuf(int fd, int bufsize);

bool is_private_ip(const string& ip);
//网络字节序整型ip转换为点分十进制字符串ip
string net_int_ip2str(uint32_t ip);
//主机字节序整型ip转换为点分十进制字符串ip
string host_int_ip2str(uint32_t ip);
//点分十进制字符串ip转换为网络字节序整型ip
uint32_t str2net_int_ip(const string& ip);
//点分十进制字符串ip转换为主机字节序整型ip
uint32_t str2host_int_ip(const string& ip);

int get_peer_name(int fd, uint32_t& peer_addr, uint16_t& peer_port);
int get_sock_name(int fd, uint32_t& peer_addr, uint16_t& peer_port);

bool get_local_ip(const char* eth_name, string& ip);
bool get_local_mac(const char* eth_name, string& mac);

int set_signal_handler(int sig_no, void (*handler)(int, siginfo_t*, void*));

#endif