#pragma once
#include <vector>
#include "co_routine.h"
//协程互斥锁
class clsCoMutex {
 public:
  clsCoMutex();
  ~clsCoMutex();

  //协程锁无法用来做公平调度，如果本协程一直无让出操作的话，是有可能导致其它协程饿死的
  //业务不能依赖协程锁释放之后，其它协程有机会被调度，这个需要依赖本协程有让出才行
  void CoLock();
  void CoUnLock();

 private:
  stCoCond_t* m_ptCondSignal;
  int m_iWaitItemCnt;
};

class clsSmartLock {
 public:
  clsSmartLock(clsCoMutex* m) {
    m_ptMutex = m;
    m_ptMutex->CoLock();
  }
  ~clsSmartLock() { m_ptMutex->CoUnLock(); }

 private:
  clsCoMutex* m_ptMutex;
};

enum {
  eWaitRead = 1,
  eWaitWrite = 2,
};

class clsPollConnection;
class clsCoConnection {
 public:
  clsCoConnection(int fd, int timeoutms);
  ~clsCoConnection();

  static void* OnEventTrigger(int fd, int revent, void* args);
  static void* OnUserReadEventCallBack(int fd, int revent, void* args);
  void* CallUserReadCallBack(int fd, int revent);

  void SetReadEventCallBackOneShot(pfn_co_event_call_back pfn, void* args,
                                   int timeout);
  void ClearReadEventCallBack();

  enum {
    eOk = 0,
    eReadErr = 1,
    eReadTimeout = 2,
    eWriteErr = 3,
    eWriteTimeout = 4,
    eEpollErr = 5,
    eEventErr = 6,
    eConnClose = 7,
  };

  void SetSockTimeout(int iTimeoutMS);

  int Read(void* buffer, size_t n);

  int Read(void* buffer, size_t n, bool async);

  int Write(void* buffer, size_t n);

  int Write(void* buffer, size_t n, bool async);

  int Ref();

  int UnRef();

  int GetConnStatus();

  int GetConnErrno();

  int Close();

  friend clsPollConnection;

 private:
  void SetEvent(int fd, int revent);
  int Wait(int mode, bool async);

  int m_iConnStatus;
  int m_iFd;
  int m_iRef;

  int m_iReadStatus;
  int m_iReadTimeOut;
  stCoCond_t* m_ptWaitReadSignal;

  int m_iWriteStatus;
  int m_iWriteTimeOut;
  stCoCond_t* m_ptWaitWriteSignal;

  pfn_co_event_call_back m_pfnOnReadEventCallBack;
  void* m_ptArgs;
  stCoEvent_t* m_ptReadEventTimer;

  int m_iLastRet;
  int m_iLastErrno;
  stCoEvent_t* m_ptCoEvent;
  clsCoMutex m_stReadLock;
  clsCoMutex m_stWriteLock;
};

struct stPollEntry_t {
  stPollEntry_t() {
    conn = NULL;
    mode = 0;
    rmode = 0;
  }
  stPollEntry_t(clsCoConnection* c, int m) {
    conn = c;
    mode = m;
    rmode = 0;
  }
  clsCoConnection* conn;
  int mode;
  int rmode;
};

class clsPollConnection {
 public:
  static int Poll(stPollEntry_t* entrys, int count, int timeout);

 private:
};

class clsNetBuffer {
 public:
  clsNetBuffer();
  ~clsNetBuffer();
  int AttachMalloc(char* buffer, int size);
  int Detach();
  int DetachBuffer(char*& buffer, int& size, int& capacity);
  char* GetWritePtr();
  int AddWritePtr(int pos);
  char* GetReadPtr();
  int AddReadPtr(int pos);
  int GetAvailSize();
  int Reserve(int size);
  int EnsureSpace(int size);
  int GetCapacity();
  int Produce(const char* buffer, int size);
  int Consume(char* buffer, int size, int& len);
  int GetUnReadSize();
  int GetWriteSize();
  int GetReadSize();
  int AdjustSpace();
  void Init();

  clsNetBuffer& operator=(clsNetBuffer& other);

 private:
  char m_sTmpBuffer[128];
  char* m_sBuffer;
  int m_iCapacity;
  int m_iReadPtr;
  int m_iWritePtr;
  bool m_bAttach;
};
