
#include "co_routine.h"
#include "co_routine_inner.h"

#include "iRoutineSpecific.h"

#include "kv6svrclient.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int keyCnt = 0;

class clsTestGlobal {
 public:
  const char *GetRandomKey() { return m_key[random() % m_key.size()].c_str(); }
  vector<string> m_key;
  const char *m_config;
  int m_thread_cnt;
  int m_routine_cnt;
};
static clsTestGlobal g_test;

struct stTestStat_t {
  long long total;
  long long empty;
  long long fail;
  long long slow;
  long long size;
  long long used;
};
static stTestStat_t *g_stat = NULL;
static void *routine_func(void *arg) {
  // CO_NEED_4
  co_enable_hook_sys();

  long thread_idx = (long)arg;
  stTestStat_t *lpStat = g_stat + thread_idx;

  string *keyList = new string[keyCnt];
  vector<string *> keyVector;
  for (int index = 0; index < keyCnt; ++index) {
    keyVector.push_back(&keyList[index]);
  }

  Kv6svrClient cli(g_test.m_config);
  for (;;) {
    if (keyCnt == 1) {
      char *value = NULL;
      int valuelen = 0;
      const char *key = g_test.GetRandomKey();
      struct timeval s, e;
      gettimeofday(&s, NULL);
      int ret = cli.GetByStr(key, &value, valuelen);
      gettimeofday(&e, NULL);

      long diff = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000;
      if (diff > 10) {
        lpStat->slow++;
      }
      lpStat->used += diff;
      if (value) {
        free(value);
        value = NULL;
      }
      lpStat->total++;
      if (1 == ret) {
        lpStat->empty++;
      } else if (ret) {
        lpStat->fail++;
      }
      lpStat->size += valuelen;
    } else {
      int valuelen = 0;
      for (int index = 0; index < keyCnt; ++index) {
        keyList[index] = g_test.GetRandomKey();
      }
      vector<KeyValue *> valueVector;

      struct timeval s, e;
      gettimeofday(&s, NULL);
      int ret = cli.BatchGetByStr(&keyVector, &valueVector);
      gettimeofday(&e, NULL);

      long diff = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000;
      if (diff > 10) {
        lpStat->slow++;
      }
      lpStat->used += diff;

      for (int index = 0; index < valueVector.size(); ++index) {
        if (valueVector[index] != NULL) {
          lpStat->size += valueVector[index]->tValue.GetLen();
          delete valueVector[index];
          valueVector[index] = NULL;
        }
      }

      lpStat->total++;
      if (1 == ret) {
        lpStat->empty++;
      } else if (ret) {
        lpStat->fail++;
      }
    }
  }
}
static void *thread_func(void *arg) {
  // CO_NEED_2
  co_init_curr_thread_env();

  long thread_idx = (long)arg;
  for (int i = 0; i < g_test.m_routine_cnt; i++) {
    stCoRoutine_t *co = NULL;
    co_create(&co, NULL, routine_func, (void *)thread_idx);
    co_resume(co);
  }

  // CO_NEED_3
  co_eventloop(co_get_epoll_ct(), NULL, 0);
  return 0;
}
int main(int argc, char *argv[]) {
  // CO_NEED_1
  RoutineSetSpecificCallback(co_getspecific, co_setspecific);

  if (argc < 4) {
    printf(
        "usage: %s /home/qspace/etc/client/shanghai/mmsnsobjkv_kvsvr_cli.conf "
        "10(thead) 10(routine) 2(batchget_unit)< key.lst\n",
        argv[0]);
    exit(0);
  }

  keyCnt = strtol(argv[4], NULL, 10);

  signal(SIGPIPE, SIG_IGN);
  srandom(time(NULL));

  char buf[1024];
  while (fgets(buf, sizeof(buf), stdin)) {
    char *lp = strpbrk(buf, " \r\n");
    if (lp) *lp = '\0';
    g_test.m_key.push_back(buf);
  }

  g_test.m_config = argv[1];
  g_test.m_thread_cnt = atoi(argv[2]);
  g_test.m_routine_cnt = atoi(argv[3]);

  g_stat =
      (stTestStat_t *)calloc(1, sizeof(stTestStat_t) * g_test.m_thread_cnt);

  pthread_t tid[g_test.m_thread_cnt];
  for (long i = 0; i < g_test.m_thread_cnt; i++) {
    pthread_create(tid + i, NULL, thread_func, (void *)i);
  }
  stTestStat_t st = {0};
  for (;;) {
    sleep(1);
    stTestStat_t now = {0};
    for (int i = 0; i < g_test.m_thread_cnt; i++) {
      now.total += g_stat[i].total;
      now.empty += g_stat[i].empty;
      now.fail += g_stat[i].fail;
      now.slow += g_stat[i].slow;
      now.size += g_stat[i].size;
      now.used += g_stat[i].used;
    }
    printf(
        "total %lld empty %lld fail %lld succ %.2lf%% slow %lld size %.2f "
        "Mbyte avg %.2lf ms\n",
        now.total - st.total, now.empty - st.empty, now.fail - st.fail,
        double(100) -
            double(now.fail - st.fail) / double(now.total - st.total) * 100,
        now.slow - st.slow, double(now.size - st.size) / 1024 / 1024,
        double(now.used - st.used) / (now.total - st.total));

    st = now;
  }

  return 0;
}
// mmkvsvrtest26[qq]:/home/qspace/upload # ./example_kvcli
// /home/qspace/etc/client/shanghai/mmsnsobjkv_kvsvr_cli.conf 16 30 < key.lst
