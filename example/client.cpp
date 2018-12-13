#include "client.h"
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string>
using namespace std;
#include "const.h"
#include "printf.h"
#include "socket_option.h"

client *client::g_client = new client;

client::client()
{
	m_epoll_fd = -1;
	m_tcp_fd = -1;
	m_tcp_conn = NULL;
	m_udp_fd = -1;
	m_unix_fd = -1;
	m_raw_fd = -1;
	m_flag = true;
}

client::~client()
{
	stop();
}

client *client::get_instance()
{
	return g_client;
}

void client::signal_handler_t(int signum)
{
	if (signum == SIGPIPE)
	{
		PRINTF_DEBUG("recv SIGPIPE signal");
	}
	if (signum == SIGINT)
	{
		PRINTF_DEBUG("recv SIGINT signal");
		g_client->stop();
	}
	if (signum == SIGTERM)
	{
		PRINTF_DEBUG("recv SIGTERM signal");
	}
}

int client::start()
{
	m_epoll_fd = epoll_create1(0);
	if (-1 == m_epoll_fd)
	{
		PRINTF_ERROR("epoll_create1 error");
		return -1;
	}
	if (SIG_ERR == signal(SIGPIPE, signal_handler_t))
	{
		PRINTF_ERROR("signal error");
		return -1;
	}
	if (SIG_ERR == signal(SIGINT, signal_handler_t))
	{
		PRINTF_ERROR("signal error");
		return -1;
	}
	if (SIG_ERR == signal(SIGTERM, signal_handler_t))
	{
		PRINTF_ERROR("signal error");
		return -1;
	}
	if (-1 == tcp_socket_start())
	{
		PRINTF_ERROR("tcp_socket_start error");
		return -1;
	}
	if (-1 == udp_socket_start())
	{
		PRINTF_ERROR("udp_socket_start error");
		return -1;
	}
	event_loop();
	return 0;
}

void client::stop()
{
	m_flag = false;
	if (m_epoll_fd != -1)
	{
		close(m_epoll_fd);
		m_epoll_fd = -1;
	}
	if (m_tcp_fd != -1)
	{
		close(m_tcp_fd);
		m_tcp_fd = -1;
	}
	if (m_tcp_conn != NULL)
	{
		delete m_tcp_conn;
		m_tcp_conn = NULL;
	}
	if (m_udp_fd != -1)
	{
		close(m_udp_fd);
		m_udp_fd = -1;
	}
	if (m_unix_fd != -1)
	{
		close(m_unix_fd);
		m_unix_fd = -1;
	}
	if (m_raw_fd != -1)
	{
		close(m_raw_fd);
		m_raw_fd = -1;
	}
}

int client::tcp_socket_start()
{
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_LISTEN_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	PRINTF_DEBUG("server ip:%s", SERVER_IP);
	m_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int ret = connect_timeout(m_tcp_fd, (struct sockaddr*)&server_addr, sizeof(server_addr), 3, 0);
	if (ret == -1)
	{
		PRINTF_ERROR("connect error");
		close(m_tcp_fd);
		m_tcp_fd = -1;
		return -1;
	}
	struct epoll_event event;
	event.data.fd = m_tcp_fd;
	event.events = EPOLLIN | EPOLLET;
	if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_tcp_fd, &event))
	{
		PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_ADD, %d) error", m_epoll_fd, m_tcp_fd);
		return -1;
	}
	m_tcp_conn = new connector(m_tcp_fd, inet_ntoa(server_addr.sin_addr), NULL);
	return 0;
}

int client::connect_timeout(int fd, const struct sockaddr *addr, socklen_t addrlen, int nsec, int usec)
{
	if (-1 == make_socket_nonblocking(fd))
	{
		PRINTF_ERROR("make_socket_nonblocking(%d) error", fd);
		return -1;
	}
	struct timeval timeout;
	timeout.tv_sec = nsec;
	timeout.tv_usec = usec;
	fd_set write_set;
	FD_ZERO(&write_set);
	FD_SET(fd, &write_set);
	int ret = connect(fd, addr, addrlen);
	if (ret == 0)
	{
		PRINTF_DEBUG("connect直接成功");
		return 0;
	}
	else if (ret == -1)
	{
		if (errno != EINPROGRESS)
		{
			PRINTF_ERROR("connect error");
			return -1;
		}
		int count = select(fd+1, NULL, &write_set, NULL, &timeout);
		if (count == 0)
		{
			PRINTF_ERROR("select timeout");
			return -1;
		}
		if (count == -1)
		{
			PRINTF_ERROR("select error");
			return -1;
		}
		if (FD_ISSET(fd, &write_set))
		{
			int err = 0;
			socklen_t len = sizeof(err);
			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
			{
				PRINTF_ERROR("getsockopt error");
				return -1;
			}
			if (err != 0)
			{
				PRINTF_ERROR("connect error");
				return -1;
			}
			return 0;
		}
		else
		{
			PRINTF_ERROR("FD_ISSET error");
			return -1;
		}
	}
}

int client::udp_socket_start()
{
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(UDP_LISTEN_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	m_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//TCP套接字调用connect会引发三次握手，而UDP套接字则不会引发三次握手，只是检查是否存在错误，然后立即返回
	int ret = connect(m_udp_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (-1 == ret)
	{
		PRINTF_DEBUG("connect ip:%s port:%d failure", SERVER_IP, TCP_LISTEN_PORT);
	}
	if (-1 == make_socket_nonblocking(m_udp_fd))
	{
		PRINTF_ERROR("make_socket_nonblocking(%d) error", m_udp_fd);
		return -1;
	}
	if (-1 == set_socket_rcvbuf(m_udp_fd, UDP_RCV_BUF))
	{
		PRINTF_ERROR("set_socket_rcvbuf(%d, %d) error", m_udp_fd, UDP_RCV_BUF);
		return -1;
	}
	if (-1 == set_socket_sndbuf(m_udp_fd, UDP_SND_BUF))
	{
		PRINTF_ERROR("set_socket_sndbuf(%d, %d) error", m_udp_fd, UDP_SND_BUF);
		return -1;
	}
	struct epoll_event event;
	event.data.fd = m_udp_fd;
	event.events = EPOLLIN | EPOLLET;
	if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_udp_fd, &event))
	{
		PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_ADD, %d) error", m_epoll_fd, m_udp_fd);
		return -1;
	}
	return 0;
}

void client::event_loop()
{
	struct epoll_event events[128];
	while(m_flag)
	{
		int count = epoll_wait(m_epoll_fd, events, 128, -1);
		if (-1 == count)
		{
			PRINTF_ERROR("epoll_wait(%d) error", m_epoll_fd);
			return;
		}
		for (int i = 0; i < count; i++)
		{
			if (events[i].events&EPOLLIN)
			{
				if (events[i].data.fd == m_tcp_fd)
				{
					do_tcp_recv();
				}
				else if (events[i].data.fd == m_udp_fd)
				{
					do_udp_recvfrom();
				}
			}
			else if(events[i].events&EPOLLOUT)
			{
				PRINTF_DEBUG("EPOLLOUT");
			}
		}
	}
}

void client::do_tcp_recv()
{
	while (true)
	{
		char buf[1024] = {0};
		ssize_t ret = recv(m_tcp_fd, buf, 1024, 0);
		if (ret > 0)
		{
			m_tcp_conn->m_buffer->append(buf, ret);
			continue;
		}
		else if (ret == -1)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				break;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				PRINTF_ERROR("fd:%d abnormal disconnection", m_tcp_fd);
				if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_tcp_fd, NULL))
				{
					PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, m_tcp_fd);
				}
				delete m_tcp_conn;
				break;
			}
		}
		if (ret == 0)
		{
			PRINTF_ERROR("fd:%d normal disconnection", m_tcp_fd);
			if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_tcp_fd, NULL))
			{
				PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, m_tcp_fd);
			}
			delete m_tcp_conn;
			break;
		}
	}
}

void client::do_tcp_send(int fd, const char *data, int len)
{
	int total_send = 0;
	while (total_send < len)
	{
		int ret = send(fd, data+total_send, len-total_send, 0);
		if (ret > 0)
		{
			total_send += ret;
			continue;
		}
		else if (ret == -1)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				break;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				PRINTF_ERROR("fd:%d ip:%s abnormal disconnection", fd, m_tcp_conn->m_ip);
				if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL))
				{
					PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, fd);
				}
				delete(m_tcp_conn);
				break;
			}
		}
		if (ret == 0)
		{
			PRINTF_ERROR("fd:%d ip:%s normal disconnection", fd, m_tcp_conn->m_ip);
			if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL))
			{
				PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, fd);
			}
			delete(m_tcp_conn);
			break;
		}
	}
}

void client::do_udp_recvfrom()
{
	while (true)
	{
		char buf[UDP_RCV_BUF] = {0};
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		ssize_t ret = recvfrom(m_udp_fd, buf, UDP_RCV_BUF, 0, (struct sockaddr *)&addr, &addr_len);
		if (ret > 0)
		{
			PRINTF_DEBUG("fd:%d recvfrom:%s data:%s", m_udp_fd, inet_ntoa(addr.sin_addr), buf);
			continue;
		}
		else if (ret == -1)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				break;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				PRINTF_ERROR("fd:%d abnormal disconnection", m_udp_fd);
				if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_udp_fd, NULL))
				{
					PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, m_udp_fd);
				}
				close(m_udp_fd);
				m_udp_fd = -1;
				break;
			}
		}
		if (ret == 0)
		{
			PRINTF_ERROR("fd:%d normal disconnection", m_udp_fd);
			if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_udp_fd, NULL))
			{
				PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, m_udp_fd);
			}
			close(m_udp_fd);
			m_udp_fd = -1;
			break;
		}
	}
}

void client::do_udp_sendto(int fd, const char *data, int len, struct sockaddr_in addr)
{
	int total_send = 0;
	socklen_t addr_len = sizeof(addr);
	while (total_send < len)
	{
		int ret = sendto(fd, data+total_send, len-total_send, 0, (struct sockaddr *)&addr, addr_len);
		if (ret > 0)
		{
			total_send += ret;
			continue;
		}
		else if (ret == -1)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
				break;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				PRINTF_ERROR("fd:%d abnormal disconnection", fd);
				if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL))
				{
					PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, fd);
				}
				close(fd);
				fd = -1;
				break;
			}
		}
		if (ret == 0)
		{
			PRINTF_ERROR("fd:%d normal disconnection", fd);
			if (-1 == epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL))
			{
				PRINTF_ERROR("epoll_ctl(%d, EPOLL_CTL_DEL, %d) error", m_epoll_fd, fd);
			}
			close(fd);
			fd = -1;
			break;
		}
	}
}
