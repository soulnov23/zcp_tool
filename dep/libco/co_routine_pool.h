#pragma once
#include <functional>
#include <stack>
#include "co_channel.h"
#include "co_routine.h"
typedef void (*PfnWorkerCallBack)(int workeridx, void* args);
struct stWorker_t;

struct stTask_t {
  // callback func
  stTask_t() {
    pfn = NULL;
    lambda_func = NULL;
    args = NULL;
    lambda = false;
  }
  stTask_t(std::function<void(int)> f) {
    pfn = NULL;
    args = NULL;
    lambda_func = f;
    lambda = true;
  }
  stTask_t(PfnWorkerCallBack p, void* a) {
    pfn = p;
    args = a;
    lambda = false;
    lambda_func = NULL;
  }
  PfnWorkerCallBack pfn;
  void* args;

  // lambda function
  std::function<void(int)> lambda_func;

  bool lambda;
};

class CoRoutinePool {
 public:
  CoRoutinePool(int worker_cnt, stCoRoutineAttr_t* attr);
  static void* RoutineFunc(void* args);

  int Run(PfnWorkerCallBack func, void* args);
  int Run(std::function<void(int)> func);

  /*
  int GetIdleWorkerCnt()
  {
      return idle_worker_cnt;
  }
  */
  bool IsRoutinePoolEmpty() { return chn_task.IsPopWaitEmpty(); }

  int GetWorkerCnt() { return total_worker_cnt; }

 private:
  void WorkerRun(stWorker_t* worker);

 private:
  int total_worker_cnt;
  int idle_worker_cnt;
  clsCoChannel<stTask_t> chn_task;
};
