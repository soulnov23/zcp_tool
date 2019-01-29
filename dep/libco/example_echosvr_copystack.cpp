#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <gflags/gflags.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <queue>

#include "co_routine.h"
#include "co_routine_inner.h"

using namespace std;
struct stEnv_t {
  // for svr
  int iSvrThreadCnt;
  int iSvrRoutineCnt;
  int iSvrShareStackCnt;

  // for cli
  int iCliThreadCnt;
  int iCliRoutineCnt;
  int iCliShareStackCnt;

  // both
  unsigned short iPort;

  bool bStop;
};
void InitEnv(stEnv_t* env) {
  env->iSvrThreadCnt = 1;
  env->iSvrRoutineCnt = 10;
  env->iSvrShareStackCnt = 1;

  env->iCliThreadCnt = 1;
  env->iCliRoutineCnt = 10;
  env->iCliShareStackCnt = 1;

  env->iPort = 12319;

  env->bStop = false;
};

struct stSvrInfo_t {
  pthread_t tid;
  int svrid;

  stEnv_t* env;
  stCoEvent_t* event;
  stCoCond_t* cond;
  queue<int> waitqueue;
};

int SetNonBlock(int fd) {
  int32_t iFlag = fcntl(fd, F_GETFL);
  if (iFlag < 0) {
    printf("socket %d get F_GETFL err. %d:%s\n", fd, errno, strerror(errno));
    return -1;
  }

  iFlag |= O_NONBLOCK;

  if (fcntl(fd, F_SETFL, iFlag) < 0) {
    printf("socket %d get F_SETFL err. %d:%s\n", fd, errno, strerror(errno));
    return -1;
  }
}

int BindSocket(int fd, unsigned short port = 0, const char* ip = "*") {
  int optval = 1;

  // set reuse addr
  int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
                       sizeof(int));
  if (ret != 0) {
    return -1;
  }

  // set reuse port
  const int reuseport = 15;
  ret = setsockopt(fd, SOL_SOCKET, reuseport, (const void*)&optval,
                   sizeof(int));  // reuse_port;
  if (ret) {
    printf("%s:%d err setsockopt ret %d errno %d %s\n", __func__, __LINE__, ret,
           errno, strerror(errno));
    return -1;
  }

  // set non block
  SetNonBlock(fd);

  // bind addr
  sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  if (strcmp(ip, "*") == 0) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    inet_aton(ip, &(addr.sin_addr));
  }
  addr.sin_port = htons(port);
  ret = bind(fd, (sockaddr*)&addr, sizeof(addr));
  if (ret != 0) {
    printf("bind fd error\n");
    return -1;
  }

  // listen;
  if (listen(fd, 10000) < 0) {
    printf("listen error\n");
    return -1;
  }
  return 0;
}

void* OnAccept(int fd, int revent, void* args) {
  stSvrInfo_t* info = (stSvrInfo_t*)args;
  stCoEvent_t* event = (stCoEvent_t*)info->event;
  if (revent & POLLIN) {
    sockaddr_in sockAddr;
    socklen_t sockLen = sizeof(sockaddr_in);
    while (true) {
      int clifd = co_accept(fd, (sockaddr*)&sockAddr, &sockLen);
      if (clifd > 0) {
        // printf("%s:%d recv clifd %d\n", __func__, __LINE__, clifd);
        info->waitqueue.push(clifd);
        co_cond_signal(info->cond);
      } else if (clifd < 0 && (errno == EAGAIN || errno == EINTR)) {
        break;
      } else {
        close(fd);
        co_free_event(event);
        info->env->bStop = true;
        break;
      }
    }
  } else if (revent & POLLHUP || revent & POLLERR) {
    co_free_event(event);
    close(fd);
    info->env->bStop = true;
  }
}

int LoopFunc(void* args) {
  stEnv_t* env = (stEnv_t*)args;
  if (env->bStop) {
    return -1;
  }
  return 0;
}

void* SvrLogic(int fd) {
  while (true) {
    char sBuf[1024];
    int ret = read(fd, sBuf, 1024);
    if (ret < 0 && errno == EAGAIN) {
      continue;
    } else if (ret <= 0) {
      break;
    }
    int iWriteLen = 0;
    while (iWriteLen < ret) {
      ret = write(fd, sBuf + iWriteLen, ret - iWriteLen);
      if (ret < 0 && errno == EAGAIN) {
        continue;
      } else if (ret <= 0) {
        return NULL;
      }
      if (ret > 0) {
        iWriteLen += ret;
      }
    }
  }
  return NULL;
}

void* SvrRoutine(void* args) {
  co_enable_hook_sys();
  stSvrInfo_t* info = (stSvrInfo_t*)args;
  while (true) {
    co_cond_timedwait(info->cond, -1);
    while (!info->waitqueue.empty()) {
      int clifd = info->waitqueue.front();
      info->waitqueue.pop();
      // set sock timeout;
      timeval tm;
      tm.tv_sec = 30;
      tm.tv_usec = 0;
      setsockopt(clifd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(timeval));
      setsockopt(clifd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(timeval));

      SvrLogic(clifd);

      close(clifd);
    }
  }
}

void* SvrThread(void* args) {
  stSvrInfo_t* info = (stSvrInfo_t*)args;
  stEnv_t* env = info->env;

  // 1. alloc cond for wait queue;
  info->cond = co_cond_alloc();

  // 2. create svr routine and use share stack
  stCoRoutineAttr_t attr = {0};
  attr.use_share_stack = 1;
  attr.stack_env = co_alloc_stackmemenv(env->iSvrShareStackCnt, 128 * 1024);

  for (int i = 0; i < env->iSvrRoutineCnt; i++) {
    stCoRoutine_t* routine = NULL;
    co_create(&routine, &attr, SvrRoutine, info);
    co_resume(routine);
  }

  // 3. create socket and listen
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("create socket error\n");
    assert(false);
  }
  int ret = BindSocket(fd, info->env->iPort);
  if (ret) {
    printf("%s:%d BindSocket err\n", __func__, __LINE__);
    assert(false);
  }

  // 4. add accept callback event;
  info->event = co_alloc_event();
  ret = co_add_event(info->event, fd, OnAccept, info, POLLIN, -1);
  assert(ret == 0);

  // 5. eventloop
  co_eventloop(co_get_epoll_ct(), LoopFunc, info->env);
  return NULL;
}

struct stCliInfo_t {
  pthread_t tid;
  stEnv_t* env;
};
struct stRoutineInfo_t {
  int iRoutineID;
  stCliInfo_t* cli_info;
};

void* ClientRoutine(void* args) {
  co_enable_hook_sys();

  stRoutineInfo_t* routine_info = (stRoutineInfo_t*)args;
  stCliInfo_t* cli_info = routine_info->cli_info;
  stEnv_t* env = cli_info->env;

  sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(env->iPort);

  while (true) {
    // 1. split connect in 1 sec;
    int connect_split = rand() % 1000 + 1;
    poll(NULL, 0, connect_split);

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. set timeout;
    timeval t;
    t.tv_sec = 10;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(timeval));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(timeval));

    // 3. connect
    int ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
    if (ret) {
      printf("%s:%d connect error ret %d errno %d %s\n", __func__, __LINE__,
             ret, errno, strerror(errno));
      close(fd);
      continue;
    }

    char sBuf[128];
    snprintf(sBuf, 128, "hello world");
    // int iBufLen = strlen(sBuf) + 1;
    int iBufLen = 128;
    while (true) {
      int iWriteLen = 0;
      while (iWriteLen < iBufLen) {
        ret = write(fd, sBuf + iWriteLen, iBufLen - iWriteLen);
        if (ret > 0) {
          iWriteLen += ret;
          continue;
        } else if (ret < 0 && errno == EAGAIN) {
          continue;
        } else {
          close(fd);
          break;
        }
      }
      if (ret <= 0) {
        break;
      }
      int iReadLen = 0;
      while (iReadLen < iBufLen) {
        ret = read(fd, sBuf + iReadLen, iBufLen - iReadLen);
        if (ret > 0) {
          iReadLen += ret;
        } else if (ret < 0 && errno == EAGAIN) {
          continue;
        } else {
          close(fd);
          break;
        }
      }
      if (ret <= 0) {
        break;
      }
      printf("%s:%d routineid %d recv %s\n", __func__, __LINE__,
             routine_info->iRoutineID, sBuf);
      poll(NULL, 0, 1000);
    }
  }
  return NULL;
}
void* CliThread(void* args) {
  stCliInfo_t* cli_info = (stCliInfo_t*)args;
  stEnv_t* env = cli_info->env;

  stCoRoutine_t* routine = NULL;
  stCoRoutineAttr_t attr = {0};
  attr.use_share_stack = 1;
  attr.stack_env = co_alloc_stackmemenv(env->iCliShareStackCnt, 128 * 1024);

  stRoutineInfo_t* routine_info = new stRoutineInfo_t[env->iCliRoutineCnt];
  for (int i = 0; i < env->iCliRoutineCnt; i++) {
    routine_info[i].cli_info = cli_info;
    routine_info[i].iRoutineID = i;
    co_create(&routine, &attr, ClientRoutine, &routine_info[i]);
    co_resume(routine);
  }

  co_eventloop(co_get_epoll_ct(), LoopFunc, env);
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  co_enable_share_stack(true);
  assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);
  assert(signal(SIGCHLD, SIG_IGN) != SIG_ERR);
  assert(signal(SIGALRM, SIG_IGN) != SIG_ERR);

  stEnv_t env;
  InitEnv(&env);

  stSvrInfo_t* svr_info = new stSvrInfo_t[env.iSvrThreadCnt];
  for (int i = 0; i < env.iSvrThreadCnt; i++) {
    svr_info[i].svrid = i;
    svr_info[i].env = &env;
    pthread_create(&svr_info[i].tid, NULL, SvrThread, &svr_info[i]);
  }

  stCliInfo_t* cli_info = new stCliInfo_t[env.iCliThreadCnt];
  for (int i = 0; i < env.iCliThreadCnt; i++) {
    cli_info[i].env = &env;
    pthread_create(&cli_info[i].tid, NULL, CliThread, &cli_info[i]);
  }

  for (int i = 0; i < env.iSvrThreadCnt; i++) {
    pthread_join(svr_info[i].tid, NULL);
  }
  for (int j = 0; j < env.iCliThreadCnt; j++) {
    pthread_join(cli_info[j].tid, NULL);
  }
  printf("exit\n");
  return 0;
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL: "
    "http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/"
    "colib/example_echosvr_copystack.cpp $ $Id: example_echosvr_copystack.cpp "
    "1651504 2016-06-28 12:44:38Z leiffyli $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
