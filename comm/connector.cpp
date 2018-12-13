#include "connector.h"
#include <string.h>
#include <assert.h>
#include <unistd.h>

connector::connector(int fd, char *ip, void *arg)
{
	m_fd = fd;
	m_buffer = new buffer;
	assert(m_buffer != NULL);
	strcpy(m_ip, ip);
	m_arg = arg;
}

connector::~connector()
{
	free();
}

void connector::free()
{
	close(m_fd);
	m_fd = -1;
	if (NULL != m_buffer)
	{
		delete m_buffer;	
		m_buffer = NULL;
	}
	memset(m_ip, 0, 20);
	if (NULL != m_arg)
	{
		delete m_arg;
		m_arg = NULL;
	}
}

