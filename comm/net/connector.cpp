#include "connector.h"
#include <string.h>
#include <assert.h>
#include <unistd.h>

connector::connector(int fd, char* ip, arg* data) {
  m_fd = fd;
  m_buffer = new buffer;
  assert(m_buffer != NULL);
  strcpy(m_ip, ip);
  m_data = data;
}

connector::~connector() { free(); }

void connector::free() {
  close(m_fd);
  m_fd = -1;
  if (NULL != m_buffer) {
    delete m_buffer;
    m_buffer = NULL;
  }
  memset(m_ip, 0, 20);
  if (NULL != m_data) {
    delete m_data;
    m_data = NULL;
  }
}
