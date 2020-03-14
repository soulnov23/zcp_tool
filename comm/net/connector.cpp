#include "connector.h"
#include <string.h>
#include <assert.h>
#include <unistd.h>

connector::connector(int fd, char* ip, arg* data) {
    m_fd = fd;
    m_buffer = new buffer;
    assert(m_buffer != nullptr);
    strcpy(m_ip, ip);
    m_data = data;
}

connector::~connector() { free(); }

void connector::free() {
    close(m_fd);
    m_fd = -1;
    if (m_buffer != nullptr) {
        delete m_buffer;
        m_buffer = nullptr;
    }
    memset(m_ip, 0, 20);
    if (m_data != nullptr) {
        delete m_data;
        m_data = nullptr;
    }
}
