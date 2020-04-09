#include "co_comm.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
using namespace std;

clsCoMutex::clsCoMutex() {
  m_ptCondSignal = co_cond_alloc();
  m_iWaitItemCnt = 0;
}

clsCoMutex::~clsCoMutex() { co_cond_free(m_ptCondSignal); }

//协程锁无法用来做公平调度，如果本协程一直无让出操作的话，是有可能导致其它协程饿死的
//业务不能依赖协程锁释放之后，其它协程有机会被调度，这个需要依赖本协程有让出才行
void clsCoMutex::CoLock() {
  if (m_iWaitItemCnt > 0) {
    m_iWaitItemCnt++;
    co_cond_timedwait(m_ptCondSignal, -1);
  } else {
    m_iWaitItemCnt++;
  }
}

void clsCoMutex::CoUnLock() {
  m_iWaitItemCnt--;
  co_cond_signal(m_ptCondSignal);
}

clsCoConnection::clsCoConnection(int fd, int timeoutms) {
  m_iFd = fd;
  m_iRef = 1;

  m_iReadStatus = 0;
  m_iReadTimeOut = timeoutms;  // 1 s
  m_ptWaitReadSignal = co_cond_alloc();

  m_iWriteStatus = 0;
  m_iWriteTimeOut = timeoutms;  // 1 s
  m_ptWaitWriteSignal = co_cond_alloc();

  m_iConnStatus = 0;

  m_iLastRet = 0;
  m_iLastErrno = 0;

  // add event
  m_ptCoEvent = co_alloc_event(fd);
  int epoll_event = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET;
  int ret = co_add_event(m_ptCoEvent, OnEventTrigger, this, epoll_event, -1);
  if (ret) {
    m_iConnStatus = eEventErr;
  }

  m_pfnOnReadEventCallBack = NULL;
  m_ptArgs = NULL;
  m_ptReadEventTimer = co_alloc_event(-1);

  //主动设置异步模式，跳过底层hook
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

void* clsCoConnection::OnEventTrigger(int fd, int revent, void* args) {
  clsCoConnection* conn = (clsCoConnection*)args;
  conn->SetEvent(fd, revent);
  return NULL;
}

void* clsCoConnection::OnUserReadEventCallBack(int fd, int revent, void* args) {
  clsCoConnection* conn = (clsCoConnection*)args;
  conn->CallUserReadCallBack(fd, revent);
  return NULL;
}

void* clsCoConnection::CallUserReadCallBack(int fd, int revent) {
  co_clear_event(m_ptReadEventTimer);
  pfn_co_event_call_back pfn = this->m_pfnOnReadEventCallBack;
  void* args = this->m_ptArgs;
  int curr_fd = this->m_iFd;
  this->m_pfnOnReadEventCallBack = NULL;
  this->m_ptArgs = NULL;
  revent = 0;
  if (this->m_iReadStatus > 0) {
    revent |= EPOLLIN;
  }
  if (this->m_iWriteStatus > 0) {
    revent |= EPOLLOUT;
  }

  pfn(curr_fd, revent, args);

  return NULL;
}

void clsCoConnection::SetReadEventCallBackOneShot(pfn_co_event_call_back pfn,
                                                  void* args, int timeout) {
  this->m_pfnOnReadEventCallBack = pfn;
  this->m_ptArgs = args;
  // add timer;
  co_add_event(m_ptReadEventTimer, OnUserReadEventCallBack, this, 0, timeout);
  if (m_iReadStatus > 0) {
    co_active_event(m_ptReadEventTimer);
  }
}

void clsCoConnection::ClearReadEventCallBack() {
  co_clear_event(m_ptReadEventTimer);
  this->m_pfnOnReadEventCallBack = NULL;
  this->m_ptArgs = NULL;
  return;
}

void clsCoConnection::SetSockTimeout(int iTimeoutMS) {
  m_iReadTimeOut = iTimeoutMS;
  m_iWriteTimeOut = iTimeoutMS;
}

void clsCoConnection::SetEvent(int fd, int revent) {
  if (revent & EPOLLERR || revent & EPOLLHUP) {
    m_iReadStatus = 1;
    m_iWriteStatus = 1;
    if (m_pfnOnReadEventCallBack) {
      co_active_event(m_ptReadEventTimer);
    }
    co_cond_broadcast(m_ptWaitReadSignal);
    co_cond_broadcast(m_ptWaitWriteSignal);
  } else {
    if (revent & EPOLLOUT) {
      m_iWriteStatus = 1;
      co_cond_broadcast(m_ptWaitWriteSignal);
    }
    if (revent & EPOLLIN) {
      m_iReadStatus = 1;
      if (m_pfnOnReadEventCallBack) {
        co_active_event(m_ptReadEventTimer);
      }
      co_cond_broadcast(m_ptWaitReadSignal);
    }
  }
}

clsCoConnection::~clsCoConnection() {
  m_iRef--;

  if (m_ptWaitReadSignal) {
    co_cond_free(m_ptWaitReadSignal);
    m_ptWaitReadSignal = NULL;
  }
  if (m_ptWaitWriteSignal) {
    co_cond_free(m_ptWaitWriteSignal);
    m_ptWaitWriteSignal = NULL;
  }

  this->m_pfnOnReadEventCallBack = NULL;
  this->m_ptArgs = NULL;
  if (m_ptReadEventTimer) {
    co_free_event(m_ptReadEventTimer), m_ptReadEventTimer = NULL;
  }

  if (m_ptCoEvent) {
    co_free_event(m_ptCoEvent), m_ptCoEvent = NULL;
  }

  if (m_iFd > 0) {
    close(m_iFd), m_iFd = -1;
  }
}

int clsCoConnection::Close() {
  this->m_pfnOnReadEventCallBack = NULL;
  this->m_ptArgs = NULL;
  if (m_ptReadEventTimer) {
    co_free_event(m_ptReadEventTimer), m_ptReadEventTimer = NULL;
  }

  if (m_ptCoEvent) {
    co_free_event(m_ptCoEvent), m_ptCoEvent = NULL;
  }
  if (m_iFd > 0) {
    close(m_iFd), m_iFd = -1;
  }
  m_iLastRet = -1;
  m_iConnStatus = eConnClose;
  co_cond_broadcast(m_ptWaitReadSignal);
  co_cond_broadcast(m_ptWaitWriteSignal);
  return 0;
}

int clsCoConnection::Wait(int mode, bool async) {
  if (m_iConnStatus) {
    errno = m_iLastErrno;
    return m_iLastRet;
  }
  if (mode == eWaitRead) {
    if (m_iReadStatus > 0)  // can read
    {
      return 0;
    }
    if (async || co_is_main()) {
      errno = EAGAIN;
      return -1;
    }
    co_cond_timedwait(m_ptWaitReadSignal, m_iReadTimeOut);
    if (m_iConnStatus) {
      // conn err
      errno = m_iLastErrno;
      return m_iLastRet;
    }
    if (m_iReadStatus == 0) {
      m_iConnStatus = eReadTimeout;
      errno = EAGAIN;
      return -1;  // timeout
    }
    // can read
    return 0;
  } else if (mode == eWaitWrite) {
    if (m_iWriteStatus > 0) {
      return 0;
    }
    if (async || co_is_main()) {
      errno = EAGAIN;
      return -1;
    }
    co_cond_timedwait(m_ptWaitWriteSignal, m_iWriteTimeOut);
    if (m_iConnStatus) {
      errno = m_iLastErrno;
      return m_iLastRet;
    }
    if (m_iWriteStatus == 0) {
      m_iConnStatus = eWriteTimeout;
      errno = EAGAIN;
      return -1;  // timeout;
    }
    return 0;
  }
  return -1;
}

int clsCoConnection::Read(void* buffer, size_t n) {
  return this->Read(buffer, n, false);
}

int clsCoConnection::Read(void* buffer, size_t n, bool async) {
  clsSmartLock l(&m_stReadLock);
  int ret = 0;
  while (true) {
    ret = Wait(eWaitRead, async);
    if (ret) {
      break;
    }
    ret = read(m_iFd, buffer, n);
    if (ret > 0) {
      // read success
      break;
    } else if (ret < 0 && errno == EAGAIN) {
      // eagain
      m_iReadStatus = 0;
      continue;
    } else {
      // read err
      m_iLastErrno = errno;
      m_iLastRet = ret;
      if (ret == 0) {
        m_iConnStatus = eConnClose;
      } else {
        m_iConnStatus = eReadErr;
      }
      // write err, signal all read/write/poll co
      co_cond_broadcast(m_ptWaitWriteSignal);
      co_cond_broadcast(m_ptWaitReadSignal);
      break;
    }
  }
  return ret;
}

int clsCoConnection::Write(void* buffer, size_t n) {
  return this->Write(buffer, n, false);
}

int clsCoConnection::Write(void* buffer, size_t n, bool async) {
  clsSmartLock l(&m_stWriteLock);
  size_t iWritePtr = 0;
  int ret = -1;
  while (iWritePtr < n) {
    ret = Wait(eWaitWrite, async);
    if (ret) {
      break;
    }
    ret = write(m_iFd, (char*)buffer + iWritePtr, n - iWritePtr);
    if (ret > 0) {
      iWritePtr += ret;
    } else if (ret < 0 && errno == EAGAIN) {
      // eagain
      m_iWriteStatus = 0;
      continue;
    } else {
      m_iLastErrno = errno;
      m_iLastRet = ret;
      if (ret == 0) {
        m_iConnStatus = eConnClose;
      } else {
        m_iConnStatus = eWriteErr;
      }
      // write err, signal all read/write/poll co
      co_cond_broadcast(m_ptWaitWriteSignal);
      co_cond_broadcast(m_ptWaitReadSignal);
      break;
    }
  }
  if (ret > 0) {
    return iWritePtr;
  }
  return ret;
}

int clsCoConnection::Ref() {
  m_iRef++;
  return 0;
}

int clsCoConnection::UnRef() {
  m_iRef--;
  if (m_iRef == 0) {
    delete this;
  }
  return 0;
}

int clsCoConnection::GetConnStatus() { return m_iConnStatus; }

int clsCoConnection::GetConnErrno() { return m_iLastErrno; }

int clsPollConnection::Poll(stPollEntry_t* entrys, int count, int timeout) {
  if (count <= 0) {
    return 0;
  }
  std::vector<stPollCond_t> wait_conds;
  std::vector<int> wait_cond_index;
  for (int i = 0; i < count; i++) {
    clsCoConnection* conn = entrys[i].conn;
    int mode = entrys[i].mode;
    if (mode & eWaitRead) {
      if (conn->m_iReadStatus == 0) {
        wait_conds.emplace_back(conn->m_ptWaitReadSignal);
        wait_cond_index.emplace_back(i);
      } else {
        entrys[i].rmode |= eWaitRead;
      }
    }
    if (mode & eWaitWrite) {
      if (conn->m_iWriteStatus == 0) {
        wait_conds.emplace_back(conn->m_ptWaitWriteSignal);
        wait_cond_index.emplace_back(i);
      } else {
        entrys[i].rmode |= eWaitWrite;
      }
    }
  }
  if (wait_conds.size() < (size_t)count) {
    int event_count = count - wait_conds.size();
    return event_count;
  }
  int ret = co_cond_poll(&wait_conds[0], wait_conds.size(), timeout);
  if (ret > 0) {
    for (size_t i = 0; i < wait_conds.size(); i++) {
      if (wait_conds[i].active) {
        int index = wait_cond_index[i];
        int mode = entrys[i].mode;
        clsCoConnection* conn = entrys[i].conn;
        if (mode & eWaitRead && conn->m_iReadStatus != 0) {
          entrys[i].rmode |= eWaitRead;
        }
        if (mode & eWaitWrite && conn->m_iWriteStatus != 0) {
          entrys[i].rmode |= eWaitWrite;
        }
      }
    }
  }
  return ret;
}

clsNetBuffer::clsNetBuffer() { this->Init(); }

clsNetBuffer::~clsNetBuffer() {
  if (m_bAttach) {
    this->Detach();
  }
  if (m_sTmpBuffer != m_sBuffer) {
    if (m_sBuffer) {
      free(m_sBuffer), m_sBuffer = NULL;
      m_iCapacity = 0;
      m_iReadPtr = 0;
      m_iWritePtr = 0;
    }
  }
}
void clsNetBuffer::Init() {
  m_sBuffer = m_sTmpBuffer;
  m_iCapacity = 128;
  m_iReadPtr = 0;
  m_iWritePtr = 0;
  m_bAttach = false;
}
char* clsNetBuffer::GetWritePtr() {
  char* ptr = &m_sBuffer[m_iWritePtr];
  return ptr;
}
int clsNetBuffer::AddWritePtr(int pos) {
  m_iWritePtr += pos;
  return 0;
}
char* clsNetBuffer::GetReadPtr() {
  char* ptr = &m_sBuffer[m_iReadPtr];
  return ptr;
}
int clsNetBuffer::AddReadPtr(int pos) {
  m_iReadPtr += pos;
  return 0;
}
int clsNetBuffer::GetAvailSize() { return m_iCapacity - m_iWritePtr; }
int clsNetBuffer::Reserve(int size) {
  if (size > m_iCapacity) {
    if (m_sBuffer == m_sTmpBuffer) {
      m_sBuffer = (char*)malloc(size);
      memcpy(m_sBuffer, m_sTmpBuffer, 128);
    } else {
      m_sBuffer = (char*)realloc(m_sBuffer, size);
    }
    m_iCapacity = size;
  }
  return 0;
}
int clsNetBuffer::EnsureSpace(int size) {
  if ((m_iCapacity - m_iWritePtr) < size) {
    this->Reserve(m_iWritePtr + size);
  }
  return 0;
}
int clsNetBuffer::Produce(const char* buffer, int size) {
  EnsureSpace(size);
  char* pos = GetWritePtr();
  memcpy(pos, buffer, size);
  m_iWritePtr += size;
  return 0;
}
int clsNetBuffer::Consume(char* buffer, int size, int& len) {
  char* pos = GetReadPtr();
  int unread_size = m_iWritePtr - m_iReadPtr;
  if (unread_size > size) {
    memcpy(buffer, pos, size);
    m_iReadPtr += size;
    len = size;
  } else {
    memcpy(buffer, pos, unread_size);
    m_iReadPtr += unread_size;
    len = unread_size;
  }
  return 0;
}

int clsNetBuffer::GetUnReadSize() {
  int unread_size = m_iWritePtr - m_iReadPtr;
  return unread_size;
}

int clsNetBuffer::AdjustSpace() {
  if (m_iReadPtr == m_iWritePtr) {
    m_iReadPtr = 0;
    m_iWritePtr = 0;
  }
  int ensure_avail_size = m_iCapacity / 3;
  if (m_iReadPtr > ensure_avail_size) {
    int avail_size = GetAvailSize();
    if (avail_size < ensure_avail_size) {
      char* read_ptr = GetReadPtr();
      char* write_ptr = GetWritePtr();
      memcpy(m_sBuffer, read_ptr, write_ptr - read_ptr);
      m_iWritePtr = write_ptr - read_ptr;
      m_iReadPtr = 0;
    }
  }
  this->EnsureSpace(512);
  return 0;
}
int clsNetBuffer::GetWriteSize() { return m_iWritePtr; }
int clsNetBuffer::GetReadSize() { return m_iReadPtr; }
int clsNetBuffer::AttachMalloc(char* buffer, int size) {
  if (m_sBuffer) {
    if (m_sBuffer != m_sTmpBuffer) {
      free(m_sBuffer), m_sBuffer = NULL;
    }
  }
  m_sBuffer = buffer;
  m_iCapacity = size;
  m_iWritePtr = size;
  m_iReadPtr = 0;
  m_bAttach = true;
  return 0;
}

int clsNetBuffer::Detach() {
  if (m_sBuffer && m_sBuffer != m_sTmpBuffer) {
    m_sBuffer = m_sTmpBuffer;
    m_iReadPtr = 0;
    m_iWritePtr = 0;
    m_iCapacity = 128;
    m_bAttach = false;
  }
  return 0;
}

int clsNetBuffer::DetachBuffer(char*& buffer, int& size, int& capacity) {
  if (m_sBuffer) {
    if (m_sBuffer != m_sTmpBuffer) {
      buffer = m_sBuffer;
    } else {
      buffer = (char*)malloc(m_iCapacity);
      memcpy(buffer, m_sBuffer, m_iCapacity);
    }
    capacity = m_iCapacity;
    size = m_iWritePtr;
  }
  this->Init();
  return 0;
}

clsNetBuffer& clsNetBuffer::operator=(clsNetBuffer& other) {
  if (m_sBuffer && m_sBuffer != m_sTmpBuffer) {
    free(m_sBuffer);
    this->Init();
  }
  if (other.m_sBuffer == other.m_sTmpBuffer) {
    this->Produce(other.m_sBuffer, other.m_iWritePtr);
  } else {
    m_sBuffer = other.m_sBuffer;
    m_iReadPtr = other.m_iReadPtr;
    m_iWritePtr = other.m_iWritePtr;
    m_iCapacity = other.m_iCapacity;
  }
  other.Init();
  return *this;
}
int clsNetBuffer::GetCapacity() { return m_iCapacity; }

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL: "
    "http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/"
    "colib/co_comm.cpp $ $Id: co_comm.cpp 2881429 2018-12-19 09:42:14Z "
    "princewen $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

