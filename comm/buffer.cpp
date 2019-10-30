#include "buffer.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>

#define BUFFER_DEFAULT_SIZE 64*1024

buffer::buffer() {
	m_buffer = (char*)malloc(BUFFER_DEFAULT_SIZE);
	assert(m_buffer != NULL);
	m_size = 0;
	m_max_size = BUFFER_DEFAULT_SIZE;
	memset(m_buffer, 0, BUFFER_DEFAULT_SIZE);
}

buffer::~buffer() {
	if (NULL != m_buffer) {
		free(m_buffer);	
		m_buffer = NULL;
		m_size = 0;
		m_max_size = BUFFER_DEFAULT_SIZE;
	}
}

void buffer::append(const char* data, int len) {
	if ((m_size + len) >= m_max_size) {
		m_buffer = (char*)realloc((void*)m_buffer, 2*m_max_size);
		assert(m_buffer != NULL);
		m_max_size = 2*m_max_size;
	}
	memcpy(m_buffer, data, len);
	m_size += len;
}

void buffer::remove(int len) {
	assert(len <= m_size);
	if (len == m_size) {
		memset(m_buffer, 0, m_size);
		m_size = 0;
		return;
	}
	memmove(m_buffer+len, m_buffer, m_size-len);
	m_size = m_size - len;
}

int buffer::size() {
	return m_size;
}

