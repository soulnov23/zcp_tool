/*
        Author:sunny
        Date:2013.8
*/
#ifndef __CO_ROUTINE_H__
#define __CO_ROUTINE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <ucontext.h>
#include <unistd.h>
#include <vector>

// 1.struct

struct stCoRoutine_t;
struct stStackMemEnv_t;
struct stCoRoutineAttr_t {
  int stack_size;
  char no_protect_stack;
  char use_share_stack;
  char auto_destroy;
  stStackMemEnv_t *stack_env;
  // char reserve[ 58 ];

} __attribute__((packed));

struct stCoRoutineStat_t {
  unsigned long long hit_count;
  unsigned long long not_hit_count;

  unsigned long long alloc_buffer_size;

  unsigned long long copy_out_size;
  unsigned long long copy_out_count;
  unsigned long long copy_in_size;
  unsigned long long copy_in_count;

  unsigned long long copy_resume_count;
  unsigned long long normal_resume_count;
  unsigned long long yield_cnt;
  unsigned long long use_share_stack_cnt;
};

struct stCoEpoll_t;
typedef int (*pfn_co_eventloop_t)(void *);
typedef void *(*pfn_co_routine_t)(void *);
typedef void *(*pfn_sys_getspecific_t)(pthread_key_t key);
typedef int (*pfn_sys_setspecific_t)(pthread_key_t key, const void *value);

typedef void (*pfn_co_eventloop_start_t)(int iEventCnt);
typedef void (*pfn_co_eventloop_end_t)();
typedef void (*pfn_before_yield_cb_t)(void *);

// 2.co_routine

int co_create(stCoRoutine_t **co, const stCoRoutineAttr_t *attr,
              void *(*routine)(void *), void *arg);

void co_resume(stCoRoutine_t *co);
void co_yield(stCoRoutine_t *co);
void co_yield_ct();                            // ct = current thread
void co_yield_resume_ct(stCoRoutine_t *dest);  // ct = current thread
void co_release(stCoRoutine_t *co);
int co_add_persist_event(stCoEpoll_t *ctx, int fd, int events);

void co_free(stCoRoutine_t *co);

int co_yield_timeout(int timeout = 30000);
void co_yield_pending();
stCoRoutine_t *co_self();
bool co_is_main();

int co_poll(stCoEpoll_t *ctx, struct pollfd fds[], nfds_t nfds, int timeout_ms);
void co_eventloop(stCoEpoll_t *ctx, pfn_co_eventloop_t pfn, void *arg);

void co_reset_epolltimeout();

// 3.specific

int co_setspecific(pthread_key_t key, const void *value);
void *co_getspecific(pthread_key_t key);
void co_set_sys_specific_func(pfn_sys_getspecific_t get,
                              pfn_sys_setspecific_t set);
void co_set_eventloop_stat(pfn_co_eventloop_start_t start,
                           pfn_co_eventloop_end_t end);
void co_set_before_yield_cb(pfn_before_yield_cb_t cb, void *arg);

// 4.event

stCoEpoll_t *co_get_epoll_ct();  // ct = current thread

// 5.hook syscall ( poll/read/write/recv/send/recvfrom/sendto )

void co_enable_hook_sys();
void co_disable_hook_sys();
bool co_is_enable_sys_hook();

void co_log_err(const char *fmt, ...);

void co_set_env_list(const char *name[], size_t cnt);

// 6.sync
struct stCoCond_t;

stCoCond_t *co_cond_alloc();
int co_cond_free(stCoCond_t *cc);
bool co_cond_empty(stCoCond_t *si);

int co_cond_signal(stCoCond_t *);
int co_cond_broadcast(stCoCond_t *);
int co_cond_timedwait(stCoCond_t *, int);
int co_cond_timedwait(stCoCond_t *, int, int *);
int co_cond_kill(stCoCond_t *si, int sig);

struct stPollCond_t {
  stPollCond_t() {
    this->cond = NULL;
    this->active = false;
  }
  stPollCond_t(stCoCond_t *cond) {
    this->cond = cond;
    this->active = false;
  }
  stCoCond_t *cond;
  bool active;
};

int co_cond_poll(stPollCond_t *cond_list, int count, int timeout);

typedef void (*pfn_co_before_yield)();
typedef void (*pfn_co_after_yield)();
void co_set_yield_func(pfn_co_after_yield before, pfn_co_after_yield after);

// protect
void SetUseProtect(bool bUseProtect);
void co_enable_share_stack(bool bEnableShareStack);

// for copy stack
stCoRoutineStat_t *co_get_curr_stat();
stStackMemEnv_t *co_alloc_stackmemenv(int iCount, int iStackSize);

// event
struct stCoEvent_t;
typedef void *(*pfn_co_event_call_back)(int fd, int revent, void *args);
stCoEvent_t *co_alloc_event(int fd);
int co_free_event(stCoEvent_t *coevent);
int co_get_eventfd(stCoEvent_t *coevent);
int co_add_event(stCoEvent_t *coevent, pfn_co_event_call_back pfn, void *args,
                 unsigned int events, int timeout);
void co_clear_event(stCoEvent_t *lp);

// active
void co_active_event(stCoEvent_t *ev);

int co_get_routine_cnt();
void co_reset_epoll_ct();

std::vector<unsigned long long> co_get_time_stat();
std::vector<stCoRoutine_t *> co_get_pointer_stat();

#include <pthread.h>
class clsMultiThreadCond {
 public:
  clsMultiThreadCond();
  ~clsMultiThreadCond();
  /*
   * @timeout <= 0: syncwait; > 0 wait to timeout;
   */
  int WaitCond(int timeout);
  /*
   * @sync, sync = true, will block until there is a waiting co;
   *             = false, signal once ;
   */
  int Signal(bool sync = false);

 private:
  stCoCond_t *m_ptCoCond;
  pthread_mutex_t *m_ptCondMutex;
};

#endif
