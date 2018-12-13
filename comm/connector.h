#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "buffer.h"

class connector
{
public:
	connector(int fd, char *ip, void *arg);
	~connector();

private:
	void free();
	
public:
	int m_fd;
	buffer *m_buffer;
	char m_ip[20];
	void *m_arg;
};

#endif