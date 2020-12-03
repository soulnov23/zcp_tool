#ifndef __CO_ROUTINE_INNER_H__
#define __CO_ROUTINE_INNER_H__

#include "co_routine.h"
#include "coctx.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <list>

struct stStackMem_t;
struct stStackMemEnv_t;

struct stCoRoutineEnv_t {
  stCoRoutine_t* pCallStack[128];
  int iCallStackSize;
  stCoEpoll_t* pEpoll;

  // for copy stack
  stStackMemEnv_t* pMemEnv;
  // for stat
  stCoRoutineStat_t* pStat;

  // for copy stack log lastco and nextco
  stCoRoutine_t* ptNextCo;
  stCoRoutine_t* ptLastCo;

  stCoRoutine_t* arrCoRoutine[20480];
  int iCoRoutineCnt;
};

struct stCoSpec_t
{
	void *value;
};
struct stCoRoutine_t
{
	stCoRoutineEnv_t *env;
	pfn_co_routine_t pfn;
	void *arg;

	std::function<void()> f;

	//ucontext_t ctx;
	coctx_t ctx;
	char cStart;
	char cEnd;

	//stCoSpec_t aSpecInner[ 128 ];
	//stCoSpec_t aSpecInner2[ 128 ];
	//stCoSpec_t *aSpecPtr[ 4096 / 128 ];// 2 ^ 7
	stCoSpec_t **aSpecPtr;

	char cIsMain;
	char cEnableSysHook;
	char cAutoDestroy;

	void *pvEnv;

	char cNoProtectStack;
	char cUseSharedStack;
	
	stStackMem_t* pDynamicStack;

	//for copy stack;
	stStackMemEnv_t* pStackEnv;
	char *pRunStackSP;
	char *pBuffer;
	int  iStackCapacity;
	int  iStackLen;

	//char sRunStack[ 1 ];  //the last field!!
	/*************/
	/* forbidden */
	/*************/
	//in the hell
	char *pRunStack;

};



//1.env
void 				co_init_curr_thread_env();
stCoRoutineEnv_t *	co_get_curr_thread_env();

//2.coroutine
void    co_free( stCoRoutine_t * co );
void    co_yield_env(  stCoRoutineEnv_t *env );
void    co_yield_env_inner(  stCoRoutineEnv_t *env, bool bAutoDestroy );

//3.func


//-----------------------------------------------------------------------------------------------
struct stTimeout_t;
struct stTimeoutItem_t;
struct stTimeoutItemLink_t;
typedef void (*OnPreparePfn_t)(stTimeoutItem_t*, struct epoll_event&,
                               stTimeoutItemLink_t*);
typedef void (*OnProcessPfn_t)(stTimeoutItem_t*);

struct stTimeoutItem_t {
  enum {
    eMaxTimeout = 40 * 1000  // 40s
  };
  stTimeoutItem_t* pPrev;
  stTimeoutItem_t* pNext;
  stTimeoutItemLink_t* pLink;

  unsigned long long ullExpireTime;

  OnPreparePfn_t pfnPrepare;
  OnProcessPfn_t pfnProcess;

  void* pArg;  // routine
  bool bTimeout;

  stTimeoutItem_t()
      : pPrev(nullptr),
        pNext(nullptr),
        pLink(nullptr),
        ullExpireTime(0),
        pfnPrepare(nullptr),
        pfnProcess(nullptr),
        pArg(nullptr),
        bTimeout(false) {}
};

stTimeout_t *AllocTimeout( int iSize );
void 	FreeTimeout( stTimeout_t *apTimeout );
int  	AddTimeout( stTimeout_t *apTimeout,stTimeoutItem_t *apItem ,uint64_t allNow );

struct stCoEpoll_t;
stCoEpoll_t * AllocEpoll();
void 		FreeEpoll( stCoEpoll_t *ctx );

stCoRoutine_t *		GetCurrThreadCo();
void 				SetEpoll( stCoRoutineEnv_t *env,stCoEpoll_t *ev );

typedef void (*pfnCoRoutineFunc_t)();

int co_accept(int fd, struct sockaddr* addr, socklen_t* len);

struct stCoEpoll_t;
struct CoLockTimeoutItemLink;
struct CoLockTimeoutItem {
 public:
  CoLockTimeoutItem()
      : pPrev(nullptr),
        pNext(nullptr),
        pLink(nullptr),
        epoll_ptr(nullptr),
        active(false),
        timeout_item() {}

  int Wait(int timeout);

  void Notify();

  void OnEpollTick();

  CoLockTimeoutItem* pPrev;
  CoLockTimeoutItem* pNext;
  CoLockTimeoutItemLink* pLink;

  stCoEpoll_t* epoll_ptr;

 private:
  std::atomic<bool> active;
  stTimeoutItem_t timeout_item;

  int AddToEpoll(int timeout);
  void RemoveFromEpoll();
};

struct CoLockTimeoutItemLink {
  CoLockTimeoutItem* head;
  CoLockTimeoutItem* tail;
};

#endif

