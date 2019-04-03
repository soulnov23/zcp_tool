#include "socket_option.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "printf.h"

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


