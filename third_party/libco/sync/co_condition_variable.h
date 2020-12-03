#pragma once

#include "co_routine.h"
#include "co_routine_inner.h"
#include "lock_entry.h"
#include "spin_lock.h"
#include "macros.h"

#include <list>
#include <mutex>
#include <memory>

namespace libco {

class ConditionVariable {
 public:
  explicit ConditionVariable() = default;
  ~ConditionVariable() = default;

  void wait(std::unique_lock<std::mutex>& external_lock) {
    wait_impl(external_lock, -1);
  }

  template <typename Predicate>
  void wait(std::unique_lock<std::mutex>& external_lock, Predicate pred) {
    while (!pred()) {
      wait_impl(external_lock, -1);
    }
  }

  std::cv_status wait_for(std::unique_lock<std::mutex>& external_lock,
                          int timeout_ms) {
    return wait_impl(external_lock, timeout_ms);
  }

  template <typename Predicate>
  std::cv_status wait_for(std::unique_lock<std::mutex>& external_lock,
                          int timeout_ms, Predicate pred) {
    std::cv_status result;
    while (!pred()) {
      result = wait_impl(external_lock, timeout_ms);
      if (result == std::cv_status::timeout) {
        break;
      }
    }
    return result;
  }

  bool notify_one();

  void notify_all();

  LIBCO_NONCOPYABLE(ConditionVariable);

 private:
  // TODO(prince): lock_queue can be replaced with a lock-free version
  // Currently, we use a spinlock to protect it
  SpinLock queue_lock_;
  std::list<detail::LockEntry*> lock_queue_;

  std::cv_status wait_impl(std::unique_lock<std::mutex>& external_lock,
                           int timeout_ms);
};

}  // namespace libco
