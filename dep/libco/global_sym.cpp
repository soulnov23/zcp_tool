#include "cpp_awk.hpp"
#include "demangle.h"
int main(int argc, char *argv[]) {
  char text[1024];
  awk a(stdin, " \r\t");
  while (a.get()) {
    const char *type = a[4];
    if (  // type == strstr(type,".data")
        //|| type == strstr(type,".rodata")
        !strcmp(type, ".data")
        //|| !strcmp(type,".data.rel.ro")
        || type == strstr(type, ".bss")) {
      const char *name = a[6];
      text[0] = '\0';
      _Demangle(name, text, sizeof(text));
      // printf("ret %d text (%s) org (%s)\n", ret,text,name);
      const char *t = text;
      if (!t[0]) {
        t = name;
      }
      printf("%s %s\n", t, type);
    } else {
      // printf("type (%s)\n",type);
    }
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
    "colib/global_sym.cpp $ $Id: global_sym.cpp 500252 2013-08-22 12:24:21Z "
    "sunnyxu $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
