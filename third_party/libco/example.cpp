
#include "co_routine.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "kv6svrclient.h"
static void *routine_func(void *) {
  co_enable_hook_sys();
  Kv6svrClient cli("test.conf");
  struct timeval s, e;
  for (;;) {
    printf("get\n");
    gettimeofday(&s, 0);
    // int ret = cli.GetByStr( "sunny",&value,valuelen );
    char rsp[100];
    int rsplen = 0;
    int ret = cli.SKEcho(200000, "sunny", rsp, &rsplen);
    gettimeofday(&e, 0);
    usleep(1000);

    int64_t df = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000;
    printf("ret %d used %ld ms\n", ret, df);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  int cnt = atoi(argv[1]);

  stCoRoutine_t *arr[cnt];
  for (int i = 0; i < cnt; i++) {
    co_create(&(arr[i]), NULL, routine_func, 0);
    co_resume(arr[i]);
  }

  co_eventloop(co_get_epoll_ct(), 0, 0);
  return 0;
}
