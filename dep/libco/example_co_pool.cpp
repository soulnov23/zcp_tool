#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include "co_routine_lambda.h"
#include "co_routine_pool.h"
using namespace std;

/*
 * 使用协程池来处理生产者消费者的问题
 * ProducerThread 生产者线程，每100ms生产一个job，并放入无锁circle queue中
 * TickFunc: eventloop的回调函数，每一次回调的时候，检查circle
 * queue队列是否有元素，有则通过co_routine_pool调度协程执行
 * DoJob函数：用户定义的业务逻辑函数，最终会在co_routine_pool中的一个协程中执行。因为本函数是协程执行的，所以要注意不可有阻塞调用。
 *
*/

struct stJob_t {
  unsigned int id;
};

void DoJob(int workeridx, void* args) {
  stJob_t* job = (stJob_t*)args;
  printf("%s:%d begin to do job id %u workeridx %d\n", __func__, __LINE__,
         job->id, workeridx);
  poll(NULL, 0, 2000);
  // printf("%s:%d finish job id %d workeridx %d\n", __func__, __LINE__,
  // job->id, workeridx);
  delete job;
  return;
}

class clsCircleQueue {
  volatile long m_lHead;
  volatile long m_lTail;
  void** m_pQueue;
  int m_iQueueSize;

 public:
  explicit clsCircleQueue(int aiQueueSize) : m_lHead(0), m_lTail(0) {
    m_iQueueSize = aiQueueSize;
    m_pQueue = (void**)malloc(sizeof(void*) * aiQueueSize);
  }
  ~clsCircleQueue() {
    if (m_pQueue) {
      free(m_pQueue);
      m_pQueue = NULL;
    }
  }
  int ciPush_OneThread(void* ap) {
    if (m_lHead - m_lTail >= m_iQueueSize) {
      return 1;  // full
    }
    m_pQueue[m_lHead % m_iQueueSize] = ap;
    m_lHead++;
    return 0;
  }
  void cvTakeAll_OneThread(vector<void*>& v) {
    v.clear();
    volatile long llHead = m_lHead;

    for (volatile long i = m_lTail; i < llHead; i++) {
      // printf("idx %ld i %ld\n",i % m_iQueueSize ,i);
      v.push_back(*(m_pQueue + i % m_iQueueSize));
    }
    m_lTail = llHead;
  }
  bool cbIsEmpty() {
    volatile long lTail = m_lTail;
    if (m_lHead <= lTail) {
      return true;
    }
    return false;
  }
  int ciTake_OneThread(void** ap) {
    if (m_lHead <= m_lTail) {
      return 1;
    }
    *ap = *(m_pQueue + m_lTail % m_iQueueSize);
    m_lTail = m_lTail + 1;
    return 0;
  }
  int ciTake_MultiThread(void** ap, int aiRetry) {
    *ap = NULL;

    for (int i = 0; i < aiRetry; i++) {
      volatile long lTail = m_lTail;
      if (m_lHead <= lTail) {
        return 1;
      }

      void* lp = *(m_pQueue + lTail % m_iQueueSize);
      int ret = __sync_bool_compare_and_swap(&m_lTail, lTail, lTail + 1);
      if (ret) {
        *ap = lp;
        return 0;
      }
    }
    return -1;
  }
};

struct stTickEvent_t {
  CoRoutinePool* pool;
  clsCircleQueue* job_queue;
  bool bLambda;
  enum {
    eMaxQueueCnt = 1024,
  };
  explicit stTickEvent_t(int pool_size) {
    pool = new CoRoutinePool(pool_size, NULL);
    job_queue = new clsCircleQueue(eMaxQueueCnt);
  }
  ~stTickEvent_t() { delete pool; }
  int PushQueue(stJob_t* job) {
    int ret = job_queue->ciPush_OneThread(job);
    return ret;
  }
  int PopQueue(stJob_t** job) {
    int ret = job_queue->ciTake_MultiThread((void**)job, 10);
    return ret;
  }

  void Schedule() {
    while (true) {
      stJob_t* job = NULL;
      int ret = this->PopQueue(&job);
      if (!ret && job) {
        printf("%s:%d workercnt %d idle %d\n", __func__, __LINE__,
               pool->GetWorkerCnt(), pool->GetIdleWorkerCnt());
        if (!bLambda) {
          ret = pool->Run(DoJob, job);
        } else {
          ret = pool->Run([job](int workeridx) { DoJob(workeridx, job); });
        }
        if (ret) {
          printf("%s:%d err no worker. job id %d drop.\n", __func__, __LINE__,
                 job->id);
          delete job;
        }

      } else {
        break;
      }
    }
  }
};

void* ProducerThread(void* args) {
  stTickEvent_t* tick_event = (stTickEvent_t*)args;
  // Must use unsigned here, because signed overflow is undefined hehavior.
  unsigned int i = 0;
  while (++i != 0) {
    stJob_t* job = new stJob_t;
    job->id = i;
    int ret = tick_event->PushQueue(job);
    if (ret) {
      printf("%s:%d err push queue ret %d idx %d\n", __func__, __LINE__, ret,
             i);
    }
    poll(NULL, 0, 500);
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  stTickEvent_t* tick_event = new stTickEvent_t(2);
  tick_event->bLambda = true;

  // create Producethread
  pthread_t tid;
  pthread_create(&tid, NULL, ProducerThread, tick_event);

  co_add_background_routine([&] { tick_event->Schedule(); });

  co_eventloop(co_get_epoll_ct(), NULL, NULL);
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end
