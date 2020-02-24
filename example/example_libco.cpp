#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "net/net_utils.h"
#include "printf_utils.h"
#include "co_routine.h"
#include <iostream>
#include <stack>
using namespace std;

#define CO_ROUTINE_NUM 10
#define TCP_LISTEN_PORT 8765

class task {
 public:
  task() {}
  ~task() {
    if (-1 != m_fd) {
      close(m_fd);
      m_fd = -1;
    }
  }

 public:
  int m_fd;
  stCoRoutine_t* m_co;
};

int listen_fd;
stack<task*> task_queue;

void* do_recv(void* arg) {
  co_enable_hook_sys();
  task* task_t = (task*)arg;
  while (true) {
    if (task_t->m_fd == -1) {
      task_queue.push(task_t);
      co_yield_ct();
      continue;
    }
    PRINTF_DEBUG("do_recv");
    int fd = task_t->m_fd;
    task_t->m_fd = -1;

    struct pollfd pf = {0};
    pf.fd = fd;
    pf.events = (POLLIN | POLLERR | POLLHUP);
    co_poll(co_get_epoll_ct(), &pf, 1, 1000);
    PRINTF_DEBUG("do_recv");
    char buf[1024] = {0};
    ssize_t ret = recv(fd, buf, 1024, 0);
    if (ret > 0) {
      PRINTF_DEBUG("buffer:%s", buf);
      continue;
    } else if (ret == -1) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        break;
      } else if (errno == EINTR) {
        continue;
      } else {
        PRINTF_ERROR("fd:%d abnormal disconnection", fd);
        close(fd);
        break;
      }
    }
    if (ret == 0) {
      PRINTF_ERROR("fd:%d normal disconnection", fd);
      close(fd);
      break;
    }
  }
  return NULL;
}

void* do_accept(void* arg) {
  co_enable_hook_sys();
  while (true) {
    if (task_queue.empty()) {
      struct pollfd pf = {0};
      pf.fd = -1;
      poll(&pf, 1, 1000);
      continue;
    }
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int fd = accept(listen_fd, (struct sockaddr*)&addr, &addr_len);
    if (fd == -1) {
      struct pollfd pf = {0};
      pf.fd = listen_fd;
      pf.events = (POLLIN | POLLERR | POLLHUP);
      co_poll(co_get_epoll_ct(), &pf, 1, 1000);
      continue;
    }
    PRINTF_DEBUG("(TCP)New accept ip:%s socket:%d", inet_ntoa(addr.sin_addr),
                 fd);
    if (task_queue.empty()) {
      close(fd);
      continue;
    }
    if (-1 == make_socket_nonblocking(fd)) {
      PRINTF_ERROR("make_socket_nonblocking error");
    }
    task* task_t = task_queue.top();
    task_t->m_fd = fd;
    task_queue.pop();
    co_resume(task_t->m_co);
  }
  return NULL;
}

int do_listen() {
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(TCP_LISTEN_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == make_socket_reuseaddr(listen_fd)) {
    PRINTF_ERROR("make_socket_reuseaddr error");
    close(listen_fd);
    listen_fd = -1;
    return -1;
  }
  if (-1 ==
      bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
    PRINTF_ERROR("bind error");
    close(listen_fd);
    listen_fd = -1;
    return -1;
  }
  if (-1 == listen(listen_fd, 128)) {
    PRINTF_ERROR("listen error");
    close(listen_fd);
    listen_fd = -1;
    return -1;
  }
  if (-1 == make_socket_nonblocking(listen_fd)) {
    PRINTF_ERROR("make_socket_nonblocking error");
    close(listen_fd);
    listen_fd = -1;
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  if (-1 == do_listen()) {
    return -1;
  }
  for (int i = 0; i < CO_ROUTINE_NUM; i++) {
    task* task_t = new task;
    task_t->m_fd = -1;
    co_create(&(task_t->m_co), NULL, do_recv, task_t);
    co_resume(task_t->m_co);
  }
  stCoRoutine_t* accept_co = NULL;
  co_create(&accept_co, NULL, do_accept, NULL);
  co_resume(accept_co);
  co_eventloop(co_get_epoll_ct(), 0, 0);
  return 0;
}
