#ifndef __CO_ROUTINE_INNER_H__
#define __CO_ROUTINE_INNER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <functional>
#include "co_routine.h"
#include "coctx.h"
struct stCoRoutineEnv_t;
struct stCoSpec_t {
  void *value;
};
struct stStackMem_t;
struct stStackMemEnv_t;
struct stCoRoutine_t {
  stCoRoutineEnv_t *env;
  pfn_co_routine_t pfn;
  void *arg;

  std::function<void()> f;

  // ucontext_t ctx;
  coctx_t ctx;
  char cStart;
  char cEnd;

  // stCoSpec_t aSpecInner[ 128 ];
  // stCoSpec_t aSpecInner2[ 128 ];
  // stCoSpec_t *aSpecPtr[ 4096 / 128 ];// 2 ^ 7
  stCoSpec_t **aSpecPtr;

  char cIsMain;
  char cEnableSysHook;
  char cAutoDestroy;

  void *pvEnv;

  char cNoProtectStack;
  char cUseSharedStack;

  stStackMem_t *pDynamicStack;

  // for copy stack;
  stStackMemEnv_t *pStackEnv;
  char *pRunStackSP;
  char *pBuffer;
  int iStackCapacity;
  int iStackLen;

  // char sRunStack[ 1 ];  //the last field!!
  /*************/
  /* forbidden */
  /*************/
  // in the hell
  char *pRunStack;
};

// 1.env
void co_init_curr_thread_env();
stCoRoutineEnv_t *co_get_curr_thread_env();

// 2.coroutine
void co_free(stCoRoutine_t *co);
void co_yield_env(stCoRoutineEnv_t *env);
void co_yield_env_inner(stCoRoutineEnv_t *env, bool bAutoDestroy);

// 3.func

//-----------------------------------------------------------------------------------------------

struct stTimeout_t;
struct stTimeoutItem_t;

stTimeout_t *AllocTimeout(int iSize);
void FreeTimeout(stTimeout_t *apTimeout);
int AddTimeout(stTimeout_t *apTimeout, stTimeoutItem_t *apItem,
               uint64_t allNow);

struct stCoEpoll_t;
stCoEpoll_t *AllocEpoll();
void FreeEpoll(stCoEpoll_t *ctx);

stCoRoutine_t *GetCurrThreadCo();
void SetEpoll(stCoRoutineEnv_t *env, stCoEpoll_t *ev);

typedef void (*pfnCoRoutineFunc_t)();

int co_accept(int fd, struct sockaddr *addr, socklen_t *len);
#endif
