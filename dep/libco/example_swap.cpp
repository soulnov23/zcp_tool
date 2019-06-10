
#include "co_routine.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

static void *routine_func(void *) {
  for (;;) {
    co_yield_ct();
  }
}

int main(int argc, char *argv[]) {
  stCoRoutine_t *co = NULL;
  co_create(&co, NULL, routine_func, 0);

  struct timeval s = {0}, e = {0};
  gettimeofday(&s, 0);
  int i = 0;
  for (;;) {
    co_resume(co);
    if (i++ == 10000000) {
      i = 0;
      gettimeofday(&e, NULL);
      printf("used %ld\n",
             (e.tv_usec - s.tv_usec) / 1000 + (e.tv_sec - s.tv_sec) * 1000);
      gettimeofday(&s, 0);
    }
  }

  return 0;
}
