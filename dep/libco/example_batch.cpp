
#include "co_routine.h"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stack>
#include <vector>
#include "kv6svrclient.h"

using namespace std;
static uint32_t g_test_uin = 0;
struct batch_item_t {
  enum {
    init = 0,
    doing = 1,
    done = 2,
  };
  uint32_t uin;
  int status;
};
struct batch_job_t {
  stCoRoutine_t *job_task;
  size_t done_cnt;
  vector<batch_item_t *> task;
};
struct batch_task_t {
  stCoRoutine_t *co;
  batch_job_t *job;
};

static vector<uint32_t> g_uins;
static stack<batch_task_t *> g_idle;

static void *batch_routine(void *arg) {
  co_enable_hook_sys();
  batch_task_t *p = (batch_task_t *)arg;

  Kv6svrClient cli("/home/qspace/upload/mmnewaddrbook_kvsvr_cli.conf");
  for (;;) {
    if (!p->job) {
      g_idle.push(p);
      co_yield_ct();
      continue;
    }
    batch_job_t *job = p->job;
    p->job = 0;

    for (size_t i = 0; i < job->task.size(); i++) {
      batch_item_t *lp = job->task[i];
      if (batch_item_t::init == lp->status) {
        printf("%p doing task[ %d ]\n", co_self(), i);
        lp->status = batch_item_t::doing;
        // do(lp);
        {
          clsRecordList *lpList = NULL;
          uint32_t a, b;
          int ret = cli.Select(lp->uin, 2, NULL, NULL, &lpList, &a, &b);
          auto_ptr<clsRecordList> ptr(lpList);
          printf("--%p select %u ret %d cnt %d\n", co_self(), lp->uin, ret,
                 lpList ? lpList->ciGetRecordCount() : 0);
        }
        lp->status = batch_item_t::done;
        job->done_cnt++;
        if (job->done_cnt == job->task.size()) {
          for (size_t k = 0; k < job->task.size(); k++) {
            free(job->task[k]);
          }
          job->task.clear();
          g_idle.push(p);
          co_yield_resume_ct(job->job_task);
          break;
        }
      }
    }
  }
}
static void *routine_func(void *) {
  co_enable_hook_sys();
  Kv6svrClient cli("/home/qspace/upload/mmnewaddrbook_kvsvr_cli.conf");
  struct timeval s, e;
  for (;;) {
    printf("get\n");
    gettimeofday(&s, 0);
    // int ret = cli.GetByStr( "sunny",&value,valuelen );
    char rsp[100];
    int rsplen = 0;
    // int ret = cli.SKEcho( 200000,"sunny",rsp,&rsplen );
    clsRecordList *lpList = NULL;
    uint32_t a, b;
    int ret = cli.Select(g_test_uin, 2, NULL, NULL, &lpList, &a, &b);
    auto_ptr<clsRecordList> ptr(lpList);

    if (lpList) {
      batch_job_t job;
      job.job_task = co_self();
      job.done_cnt = 0;

      for (int i = 0; i < lpList->ciGetRecordCount(); i++) {
        clsRecord *lp = lpList->cpGetRecord(i);
        unsigned char cType = 0;
        unsigned int iLen;

        uint32_t uin = *((uint32_t *)lp->cpGetFieldValue(3, cType, iLen));
        const char *name = lp->cpGetFieldValue(2, cType, iLen);

        printf("uin %u name (%s)\n", uin, name);

        if (uin > 10000 && uin < 3000000000) {
          batch_item_t *item = (batch_item_t *)calloc(sizeof(batch_item_t), 1);
          item->uin = uin;
          item->status = batch_item_t::init;
          job.task.push_back(item);
        }
      }
      if (!job.task.empty()) {
        printf("g_idle %ld\n", g_idle.size());
        for (int i = 0; i < 10; i++) {
          if (g_idle.empty()) {
            break;
          }
          batch_task_t *lp = g_idle.top();
          g_idle.pop();
          lp->job = &job;
          co_resume(lp->co);
        }
        co_yield_ct();
      }

      printf("all done\n");
      sleep(1);
      exit(0);
    }

    int64_t df = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000;
    printf("ret %d used %ld ms\n", ret, df);
  }
  return 0;
}

#include "iRoutineSpecific.h"
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: %s uin\n", argv[0]);
    return 0;
  }
  RoutineSetSpecificCallback(co_getspecific, co_setspecific);
  signal(SIGPIPE, SIG_IGN);
  giInitRecord("/home/qspace/etc/microaddrbook_tabledef.ini");

  g_test_uin = strtoul(argv[1], NULL, 10);
  co_enable_hook_sys();

  {
    for (int i = 0; i < 10; i++) {
      batch_task_t *task = (batch_task_t *)calloc(sizeof(batch_task_t), 1);
      stCoRoutine_t *co = NULL;
      co_create(&co, NULL, batch_routine, task);
      task->co = co;
      co_resume(task->co);
    }
  }

  {
    stCoRoutine_t *arr[1];
    for (int i = 0; i < 1; i++) {
      co_create(&(arr[i]), NULL, routine_func, 0);
      co_resume(arr[i]);
    }
  }

  co_eventloop(co_get_epoll_ct(), 0, 0);
  return 0;
}
