#include "co_routine.h"
#include "co_routine_lambda.h"

#include <stdio.h>
#include <stdlib.h>

static void routine_func3() {
  printf("routine func3 start\n");
  printf("routine func3 end\n");
  return;
}

static void routine_func2() {
  printf("routine func2 start\n");

  stCoRoutineAttr_t attr = {0};
  attr.use_share_stack = true;

  co_create(&attr, routine_func3);

  printf("routine func2 end\n");

  return;
}

static void routine_func() {
  printf("routine func1 start\n");

  stCoRoutineAttr_t attr = {0};
  attr.no_protect_stack = true;

  co_create(&attr, routine_func2);

  while (true) {
    co_yield_ct();
  }

  printf("routine func1 end\n");

  return;
}

int main(int argc, char *argv[]) {
  // Note that co_create defined in co_routine_lambda.h
  // will start the coroutine automatically.

  co_enable_share_stack(true);
  for (int i = 0; i < 5; i++) {
    SetUseProtect(i % 2 == 0);
    co_create(NULL, routine_func);
  }

  return 0;
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL: "
    "http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/"
    "colib/example_auto_free.cpp $ $Id: example_auto_free.cpp 2679874 "
    "2018-07-26 02:28:04Z princewen $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
