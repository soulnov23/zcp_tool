#include "co_condition_variable.h"

#include <thread>

namespace libco {

std::cv_status ConditionVariable::wait_impl(
    std::unique_lock<std::mutex>& external_lock, int timeout) {
  detail::LockEntry* lock_entry = nullptr;
  if (!co_is_main()) {
    lock_entry = new detail::CoLockEntry();
  } else {
    lock_entry = new detail::ThreadLockEntry();
  }
  // Ref for the owner of current function
  lock_entry->Ref();

  AutoFreeHelper<detail::LockEntry> auto_free(lock_entry);
  {
    // Add current co or thread into the queue and then release
    // the external lock
    std::unique_lock<SpinLock> queue_guard(queue_lock_);
    this->lock_queue_.emplace_back(lock_entry);
    lock_entry->Ref();
  }

  // Release the external lock and wait
  external_lock.unlock();

  static const int kMaxYieldCount = 8;
  int yield_cnt = 0;
  while (true) {
    // Check if the entry has been awake
    uint8_t s = lock_entry->status.load(std::memory_order_relaxed);
    if (lock_entry->CheckIfAwake(s)) {
      external_lock.lock();
      return std::cv_status::no_timeout;
    }

    if (co_is_main() && (yield_cnt++ < kMaxYieldCount)) {
      // Force the owner to yield for several times if in thread env
      std::this_thread::yield();
      continue;
    }

    // Try to change to pending state. Enter to next iteration if failed
    if (lock_entry->SwitchToPending(s)) {
      break;
    }
  }

  auto wait_status = lock_entry->wait(timeout);

  // Remember to relock at the end
  external_lock.lock();
  return wait_status;
}

bool ConditionVariable::notify_one() {
  detail::LockEntry* lock_entry = nullptr;

  while (true) {
    {
      // Find a valid entry to wakeup
      std::unique_lock<SpinLock> queue_guard(queue_lock_);
      if (lock_queue_.empty()) {
        return false;
      }

      lock_entry = lock_queue_.front();
      lock_queue_.pop_front();
    }

    AutoFreeHelper<detail::LockEntry> auto_free(lock_entry);
    while (true) {
      // Check if the entry has been suspended
      uint8_t s = lock_entry->status.load(std::memory_order_relaxed);
      if (lock_entry->CheckIfPending(s)) {
        break;
      }

      if (lock_entry->SwitchToAwake(s)) {
        // Awake an entry which has not began to suspend
        return true;
      }
    }

    // The lock_entry must have been marked as PENDING if we reach here.
    // So we can notify it safely
    if (lock_entry->notify_one()) {
      return true;
    }
  }
  return false;
}

void ConditionVariable::notify_all() {
  detail::LockEntry* lock_entry = nullptr;

  while (true) {
    {
      // Find a valid entry to wakeup
      std::unique_lock<SpinLock> queue_guard(queue_lock_);
      if (lock_queue_.empty()) {
        return;
      }

      lock_entry = lock_queue_.front();
      lock_queue_.pop_front();
    }

    {
      AutoFreeHelper<detail::LockEntry> auto_free(lock_entry);
      bool need_notify = true;
      while (true) {
        // Check if the entry has been suspended
        uint8_t s = lock_entry->status.load(std::memory_order_relaxed);
        if (lock_entry->CheckIfPending(s)) {
          break;
        }

        if (lock_entry->SwitchToAwake(s)) {
          // Awake an entry which has not began to suspend
          need_notify = false;
          break;
        }
      }

      // The lock_entry must have been marked as PENDING if we reach here.
      // So we can notify it safely
      if (need_notify) {
        lock_entry->notify_one();
      }
    }
  }
}

}  // namespace libco

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

