#include "net_comm.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "printf.h"
#include "tool.h"

int make_socket_blocking(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFL, NULL)) == -1) 
	{
		PRINTF_ERROR("fcntl(%d, F_GETFL) error", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags&~O_NONBLOCK) == -1) 
	{
		PRINTF_ERROR("fcntl(%d, F_SETFL) error", fd);
		return -1;
	}
	return 0;
}

int make_socket_nonblocking(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFL, NULL)) == -1) 
	{
		PRINTF_ERROR("fcntl(%d, F_GETFL) error", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) 
	{
		PRINTF_ERROR("fcntl(%d, F_SETFL) error", fd);
		return -1;
	}
	return 0;
}

int make_socket_reuseaddr(int fd)
{
	int flags = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &fd, sizeof(flags));
}

int make_socket_tcpnodelay(int fd)
{
	int flags = 1;
	return setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &flags, sizeof(flags));
}

int make_socket_keepalive(int fd)
{
	int flags = 1;
	return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
}

int make_socket_cloexec(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFD, NULL)) == -1) 
	{
		PRINTF_ERROR("fcntl(%d, F_GETFD) error", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) 
	{
		PRINTF_ERROR("fcntl(%d, F_SETFD) error", fd);
		return -1;
	}
	return 0;
}

int set_socket_rcvbuf(int fd, int bufsize)
{
	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
}

int set_socket_sndbuf(int fd, int bufsize)
{
	return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
}

string net_int_ip2str(int32_t ip)
{
	struct sockaddr_in addr_in;
	char buf[128] = {0};
	addr_in.sin_addr.s_addr = ip;
	inet_ntop(AF_INET, &addr_in.sin_addr, buf, sizeof(buf));
	return buf;
}

string host_int_ip2str(int32_t ip)
{
	return net_int_ip2str(htonl(ip));
}

int32_t str2net_int_ip(const string &ip)
{
	if (ip.empty())
	{
		return 0;
	}
	vector_t vec;
	str2vec(ip, ".", vec);
	if (vec.size() != 4)
	{
		return 0;
	}
	struct sockaddr_in addr_in;
	inet_pton(AF_INET, ip.c_str(), &addr_in.sin_addr);
	return addr_in.sin_addr.s_addr;
}

int32_t str2host_int_ip(const string &ip)
{
	return ntohl(str2net_int_ip(ip));
}
