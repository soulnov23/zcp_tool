#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include <vector>
#include "co_routine.h"
using namespace std;
void* RoutineFunc(void* args) {
  int* i = (int*)args;
  printf("%d before yield\n", *i);
  co_yield_timeout(40 * (*i + 1) * 1000);
  printf("%d after yield\n", *i);
  return NULL;
}
void* RoutineFunc1(void* args) {
  co_enable_hook_sys();
  sockaddr_in in;
  in.sin_family = AF_INET;
  in.sin_port = htons(10086);
  in.sin_addr.s_addr = inet_addr("10.123.98.37");
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  int ret = connect(fd, (sockaddr*)&in, sizeof(sockaddr_in));
  printf("ret %d errno %d %s\n", ret, errno, strerror(errno));
  char buf[1024];
  ret = write(fd, buf, 1024);
  printf("write ret %d\n", ret);
  ret = read(fd, buf, 1024);
  printf("read ret %d\n", ret);
  return NULL;
}
int main(int argc, char* argv[]) {
  vector<stCoRoutine_t*> co_pool;
  int a[10];
  stCoRoutine_t* co = NULL;
  co_create(&co, NULL, RoutineFunc1, NULL);
  co_resume(co);
  for (int i = 0; i < 10; i++) {
    a[i] = i;
    co_create(&co, NULL, RoutineFunc, &a[i]);
    co_resume(co);
  }
  co_eventloop(co_get_epoll_ct(), NULL, NULL);
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
