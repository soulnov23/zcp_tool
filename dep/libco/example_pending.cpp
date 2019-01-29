
#include "co_routine.h"

#include <stdio.h>
#include <stdlib.h>

static void *active_func(void *args) {
  co_enable_hook_sys();
  stCoCond_t *cond = (stCoCond_t *)args;
  for (int i = 0; i < 3; i++) {
    printf("Active\n");
    co_cond_timedwait(cond, -1);
  }
  return 0;
}

static void *timeout_func(void *args) {
  co_enable_hook_sys();
  for (int i = 0; i < 3; i++) {
    printf("Timeout\n");
    poll(NULL, 0, 1);
  }
  return 0;
}

static void *pending_func(void *args) {
  co_enable_hook_sys();
  stCoCond_t *cond = (stCoCond_t *)args;
  for (int i = 0; i < 3; i++) {
    printf("Pending\n");
    co_cond_signal(cond);
    co_yield_pending();
  }
  return 0;
}

int main(int argc, char *argv[]) {
  stCoCond_t *cond = co_cond_alloc();

  stCoRoutine_t *active;
  co_create(&active, NULL, active_func, cond);
  co_resume(active);

  stCoRoutine_t *timeout;
  co_create(&timeout, NULL, timeout_func, 0);
  co_resume(timeout);

  stCoRoutine_t *pending;
  co_create(&pending, NULL, pending_func, cond);
  co_resume(pending);

  co_eventloop(co_get_epoll_ct(), NULL, NULL);
  return 0;
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL: "
    "http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/"
    "colib/example_pending.cpp $ $Id: example_pending.cpp 2650368 2018-07-05 "
    "04:20:13Z jerrywan $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
