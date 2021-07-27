#include "src/base/connector.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>
using namespace std;

connector::connector(int fd, char* ip, arg* data) {
    m_fd     = fd;
    m_buffer = make_shared<buffer>();
    assert(m_buffer != nullptr);
    strcpy(m_ip, ip);
    m_data = data;
}

connector::~connector() { free(); }

void connector::free() {
    close(m_fd);
    m_fd = -1;
    memset(m_ip, 0, 20);
}
