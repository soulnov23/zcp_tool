#include "co_routine_pool.h"
#include <assert.h>

class CoRoutinePool;
struct stWorker_t {
  stWorker_t() {
    idx = 0;
    pool = NULL;
  }
  int idx;
  CoRoutinePool* pool;
};

CoRoutinePool::CoRoutinePool(int worker_cnt, stCoRoutineAttr_t* attr)
    : chn_task() {
  total_worker_cnt = worker_cnt;
  idle_worker_cnt = worker_cnt;
  for (int i = 0; i < worker_cnt; i++) {
    stCoRoutine_t* co = NULL;
    stWorker_t* worker = new stWorker_t;
    worker->pool = this;
    worker->idx = i;
    co_create(&co, attr, CoRoutinePool::RoutineFunc, worker);
    co_resume(co);
  }
}

void* CoRoutinePool::RoutineFunc(void* args) {
  stWorker_t* worker = (stWorker_t*)args;
  CoRoutinePool* pool = worker->pool;
  pool->WorkerRun(worker);
  return NULL;
}

void CoRoutinePool::WorkerRun(stWorker_t* worker) {
  // enable hook
  co_enable_hook_sys();
  while (true) {
    stTask_t task;
    int ret = chn_task.SyncPop(&task);
    if (ret) {
      // printf("%s:%d pop channel err\n", __func__, __LINE__);
      continue;
    }
    idle_worker_cnt--;
    if (task.lambda) {
      task.lambda_func(worker->idx);
    } else {
      task.pfn(worker->idx, task.args);
    }
    idle_worker_cnt++;
  }
}

//输入：回调函数以及参数，回调函数无返回值
//返回值： 0 调用协程执行
//         -1 调用失败，协程池空间不够
int CoRoutinePool::Run(PfnWorkerCallBack func, void* args) {
  stTask_t worker(func, args);
  int ret = chn_task.PushToWait(worker);
  if (ret) {
    // printf("%s:%d Run with worker pool empty\n", __func__, __LINE__);
  }
  return ret;
}

int CoRoutinePool::Run(std::function<void(int)> func) {
  stTask_t worker(func);
  int ret = chn_task.PushToWait(worker);
  if (ret) {
    // printf("%s:%d Run with worker pool empty\n", __func__, __LINE__);
  }
  return ret;
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
