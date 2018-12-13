#ifndef __SOCKET_OPTION_H__
#define __SOCKET_OPTION_H__

int make_socket_blocking(int fd);

int make_socket_nonblocking(int fd);

int make_socket_reuseaddr(int fd);

int make_socket_tcpnodelay(int fd);

int set_socket_rcvbuf(int fd, int bufsize);

int set_socket_sndbuf(int fd, int bufsize);

#endif