#include "net_utils.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <string.h>
#include "printf.h"
#include "utils.h"

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

string net_int_ip2str(uint32_t ip)
{
	struct sockaddr_in addr_in;
	char buf[128] = {0};
	addr_in.sin_addr.s_addr = ip;
	inet_ntop(AF_INET, &addr_in.sin_addr, buf, sizeof(buf));
	return buf;
}

string host_int_ip2str(uint32_t ip)
{
	return net_int_ip2str(htonl(ip));
}

uint32_t str2net_int_ip(const string &ip)
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

uint32_t str2host_int_ip(const string &ip)
{
	return ntohl(str2net_int_ip(ip));
}

int get_peer_name(int fd, uint32_t &peer_addr, uint16_t &peer_port)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (getpeername(fd, (struct sockaddr*)(&addr), &len) == -1)
	{
		PRINTF_ERROR("getpeername(%d) error", fd);
		return -1;
	}
	peer_addr = addr.sin_addr.s_addr;
	peer_port = addr.sin_port;
	return 0;
}

int get_sock_name(int fd, uint32_t &peer_addr, uint16_t &peer_port)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (getsockname(fd, (struct sockaddr*)(&addr), &len) == -1)
	{
		PRINTF_ERROR("getsockname(%d) error", fd);
		return -1;
	}
	peer_addr = addr.sin_addr.s_addr;
	peer_port = addr.sin_port;
	return 0;
}

/*
bool get_local_ip(const char *eth_name, string &ip)
{
	//定义设备最大数量
	#define MAXINTERFACES 16 
	bool ret = false;

	int fd = -1;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		return ret;
	}

	//所有的设备信息数组
	struct ifreq interface[MAXINTERFACES];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(interface);
	ifc.ifc_ifcu.ifcu_buf = (caddr_t)interface;
	if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc) == -1)
	{
		close(fd);
		return ret;
	}

	int num = ifc.ifc_len/sizeof(struct ifreq);
	for (int i = num; i > 0; i--)
	{
		//设备名称
		PRINTF_DEBUG("interface name:[%s]", interface[i].ifr_ifrn.ifrn_name);
		if (strstr(interface[i].ifr_ifrn.ifrn_name, eth_name) != NULL)
		{
			if (ioctl(fd, SIOCGIFADDR, (char*)&interface[i]) == 0)
			{
				ip = net_int_ip2str((((struct sockaddr_in*)(&(interface[i].ifr_ifru.ifru_addr)))->sin_addr.s_addr));
				PRINTF_DEBUG("ip:[%s]", ip.c_str());
				ret = true;
			}
		}
	}
	close(fd);
	return ret;
}
*/

bool get_local_ip(const char *eth_name, string &ip)
{
	bool ret = false;

	int fd = -1;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		PRINTF_ERROR();
		return ret;
	}
	
	struct ifreq interface;
	strncpy(interface.ifr_ifrn.ifrn_name, eth_name, IFNAMSIZ);
	if (ioctl(fd, SIOCGIFADDR, (char*)&interface) == 0)
	{
		ip = net_int_ip2str((((struct sockaddr_in*)(&(interface.ifr_ifru.ifru_addr)))->sin_addr.s_addr));
		ret = true;
	}
	else
	{
		PRINTF_ERROR();
	}
	close(fd);
	return ret;
}

bool get_local_mac(const char *eth_name, string &mac)
{
	bool ret = false;

	int fd = -1;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		PRINTF_ERROR();
		return ret;
	}
	
	struct ifreq interface;
	strncpy(interface.ifr_ifrn.ifrn_name, eth_name, IFNAMSIZ);
	if (ioctl(fd, SIOCGIFHWADDR, (char*)&interface) == 0)
	{
		char temp_mac[20];
		snprintf(temp_mac, sizeof(temp_mac), "%02x:%02x:%02x:%02x:%02x:%02x",
			(unsigned char)interface.ifr_ifru.ifru_hwaddr.sa_data[0],
			(unsigned char)interface.ifr_ifru.ifru_hwaddr.sa_data[1],
			(unsigned char)interface.ifr_ifru.ifru_hwaddr.sa_data[2],
			(unsigned char)interface.ifr_ifru.ifru_hwaddr.sa_data[3],
			(unsigned char)interface.ifr_ifru.ifru_hwaddr.sa_data[4],
			(unsigned char)interface.ifr_ifru.ifru_hwaddr.sa_data[5]);
		mac = temp_mac;
		ret = true;
	}
	else
	{
		PRINTF_ERROR();
	}
	close(fd);
	return ret;
}

int set_signal_handle(int sig_no, void (*handle)(int, siginfo_t *, void *))
{
	struct sigaction sig;
	memset(&sig, 0, sizeof(struct sigaction));
	sig.sa_flags = SA_SIGINFO;
	sig.sa_sigaction = handle;
	if (sigaction(sig_no, &sig, nullptr) != 0)
	{
		PRINTF_ERROR();
		return -1;
	}
	return 0;
}