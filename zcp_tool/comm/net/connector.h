#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <memory>

#include "zcp_tool/comm/arg.h"
#include "zcp_tool/comm/buffer.h"
using namespace std;

class connector {
public:
    connector(int fd, char* ip, arg* data);
    ~connector();

private:
    void free();

public:
    int m_fd;
    shared_ptr<buffer> m_buffer;
    char m_ip[20];
    arg* m_data;
};

#endif