#pragma once

#include "co_routine.h"
#include "co_routine_inner.h"
#include "co_condition_variable.h"

namespace libco {

class CoMutex {
 public:
  explicit CoMutex() : notified_(false), tickets_(1) {}
  ~CoMutex() = default;

  void lock();
  bool try_lock();
  void unlock();

  LIBCO_NONCOPYABLE(CoMutex);

 private:
  std::mutex mutex_;
  ConditionVariable cv_;

  bool notified_;
  std::atomic<int> tickets_;
};

}  // namespace libco
