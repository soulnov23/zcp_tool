#include "co_routine.h"
#include "co_routine_lambda.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static void routine_func() {
  co_enable_hook_sys();

  char* name = "git.code.oa.com";

  while (true) {
    struct hostent* host = gethostbyname(name);
    poll(nullptr, 0, 1);
  }
}

int main(int argc, char* argv[]) {
  for (int i = 0; i < 5; i++) {
    co_create(nullptr, routine_func);
  }

  co_eventloop(co_get_epoll_ct(), nullptr, nullptr);

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
