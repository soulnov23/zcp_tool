
#include "co_routine.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//#include "/home/sunnyxu/QQMail/comm2/svrkit/skmemcachedclient.hpp"
#include "skmemcachedclient.hpp"

#include "kv6svrclient.h"
static const char *ip = NULL;
static int port = 0;
static int g_cnt = 0;
static int g_fail = 0;

time_t g_last = 0;
static char value[1024];
static void *routine_func(void *) {
  SKAutoRetryMemcachedClient cli(ip, port, 3);
  cli.SetExitOnError(false);
  cli.Put("test", value, sizeof(value));
  for (;;) {
    char *pcVal = NULL;
    size_t len = 0;
    int ret = cli.Get("test", &pcVal, &len);
    if (!ret && !len) {
      cli.Put("test", value, sizeof(value));
    }
    if (pcVal) {
      free(pcVal);
      pcVal = 0;
    }
    ++g_cnt;
    if (ret) {
      ++g_fail;
    }
    if (time(NULL) != g_last) {
      g_last = time(NULL);
      printf("ret %d total %d fail %d len %ld\n", ret, g_cnt, g_fail, len);

      g_cnt = 0;
      g_fail = 0;
      len = 0;
    }
  }
}

int main(int argc, char *argv[]) {
  int cnt = atoi(argv[1]);
  ip = argv[2];
  port = atoi(argv[3]);

  co_enable_hook_sys();

  stCoRoutine_t *arr[cnt];
  for (int i = 0; i < cnt; i++) {
    co_create(&(arr[i]), NULL, routine_func, 0);
    co_resume(arr[i]);
  }

  co_eventloop(co_get_epoll_ct(), 0, 0);
  return 0;
}
/*qspace@mmaccounthk17[qq]:~/upload> ./example_memcached 300 10.191.147.204
44772
init pid 20613 env 0x23158a0
ret 0 total 1 fail 0 len 6
ret 0 total 3918 fail 0 len 6
ret 0 total 8274 fail 0 len 6
ret 0 total 8338 fail 0 len 6
ret 0 total 8322 fail 0 len 6
*/
