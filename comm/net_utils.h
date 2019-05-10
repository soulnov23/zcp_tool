#ifndef __NET_COMM_H__
#define __NET_COMM_H__

#include <string>
using namespace std;
#include <unistd.h>

int make_socket_blocking(int fd);

int make_socket_nonblocking(int fd);

int make_socket_reuseaddr(int fd);

int make_socket_tcpnodelay(int fd);

int make_socket_keepalive(int fd);

int make_socket_cloexec(int fd);

int set_socket_rcvbuf(int fd, int bufsize);

int set_socket_sndbuf(int fd, int bufsize);

//网络字节序整型ip转换为点分十进制字符串ip
string net_int_ip2str(uint32_t ip);
//主机字节序整型ip转换为点分十进制字符串ip(常用)
string host_int_ip2str(uint32_t ip);
//点分十进制字符串ip转换为网络字节序整型ip
uint32_t str2net_int_ip(const string &ip);
//点分十进制字符串ip转换为主机字节序整型ip(常用)
uint32_t str2host_int_ip(const string &ip);

int get_peer_name(int fd, uint32_t &peer_addr, uint16_t &peer_port);
int get_sock_name(int fd, uint32_t &peer_addr, uint16_t &peer_port);

#endif