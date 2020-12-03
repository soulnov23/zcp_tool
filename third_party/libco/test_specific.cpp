#include "co_routine_specific.h"
#include <stdio.h>
#include <unistd.h>

struct stMyData {
  int a;
  char b[20];
};
CO_ROUTINE_SPECIFIC(stMyData, __routine)
struct stMyData2 {
  int a;
  char b[20];
};
CO_ROUTINE_SPECIFIC(stMyData2, r2)
void *TestFunc(void *args) {
  for (;;) {
    __routine->a++;
    r2->a++;
    sprintf(__routine->b, "tid%lu-%u-%u", pthread_self(), __routine->a, r2->a);
    printf("%d %s %d\n", __routine->a, __routine->b, r2->a);
    usleep(1000);
  }
}
void *test_getspecific(pthread_key_t key) {
  printf("%s\n", __func__);
  return pthread_getspecific(key);
}
int test_setspecific(pthread_key_t key, const void *value) {
  printf("%s\n", __func__);
  return pthread_setspecific(key, value);
}
int main() {
  CoRoutineSetSpecificCallback(test_getspecific, test_setspecific);

  stMyData &r = *__routine.operator->();
  stMyData *pr = __routine.operator->();

  printf("r.a %d pr->a %d\n", r.a, pr->a);
  pthread_t tThread1;
  pthread_create(&tThread1, NULL, TestFunc, NULL);
  pthread_detach(tThread1);
  pthread_t tThread2;
  pthread_create(&tThread2, NULL, TestFunc, NULL);
  pthread_detach(tThread2);
  for (;;) {
    sleep(10);
  }
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL: "
    "http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/"
    "colib/test_specific.cpp $ $Id: test_specific.cpp 505204 2013-08-30 "
    "04:12:11Z sarlmolchen $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
