#pragma once

#include "co_routine.h"
#include "co_routine_inner.h"
#include "macros.h"
#include "ref_object.h"
#include "spin_lock.h"

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>

namespace libco {

namespace detail {

enum CoLockStatus : uint8_t {
  kStatusIdle = 0,
  kStatusPending = 1,
  kStatusAwake = 1 << 1,
};

struct LockEntry : public AtomicRefObject<LockEntry> {
  enum LockEntryType {
    kRoutine = 1,
    kThread = 2,
  };

  std::atomic<uint8_t> status;
  LockEntryType type;
  SpinLock notify_lock;

  LockEntry(LockEntryType _type) : status(kStatusIdle), type(_type) {}

  virtual ~LockEntry() = default;

  bool CheckIfAwake(uint8_t& s) { return s & kStatusAwake; }

  bool CheckIfPending(uint8_t& s) { return s & kStatusPending; }

  bool SwitchToAwake(uint8_t& s) {
    return status.compare_exchange_weak(s, kStatusAwake);
  }

  bool SwitchToPending(uint8_t& s) {
    return status.compare_exchange_weak(s, kStatusPending);
  }

  virtual std::cv_status wait(int timeout) = 0;

  virtual bool notify_one() = 0;
};

struct CoLockEntry : public LockEntry {
  CoLockTimeoutItem* co_lock;

  CoLockEntry() : LockEntry(LockEntry::kRoutine) {
    co_lock = new CoLockTimeoutItem();
  }

  virtual ~CoLockEntry() {
    delete co_lock;
    co_lock = nullptr;
  }

  virtual std::cv_status wait(int timeout) {
    status.store(kStatusPending, std::memory_order_release);

    co_lock->Wait(timeout);
    if (notify_lock.try_lock()) {
      return std::cv_status::timeout;
    } else {
      return std::cv_status::no_timeout;
    }
  }

  virtual bool notify_one() {
    if (!notify_lock.try_lock()) {
      return false;
    }
    co_lock->Notify();
    status.store(kStatusAwake, std::memory_order_release);
    return true;
  }

} LIBCO_CACHELINE_ALIGNMENT;

struct ThreadLockEntry : public LockEntry {
  // Make CoMutex works well in an environment where threads
  // with or without coroutines contention for some resources
  std::mutex thread_mutex;
  std::condition_variable thread_cv;

  ThreadLockEntry() : LockEntry(LockEntry::kThread) {}

  virtual ~ThreadLockEntry() = default;

  virtual std::cv_status wait(int timeout) override {
    // Not that thread_mutex is necessary here to ensure current thread will
    // thuely wait on the cv
    uint8_t s = status.load(std::memory_order_relaxed);
    if (CheckIfAwake(s)) {
      return std::cv_status::no_timeout;
    }
    std::unique_lock<std::mutex> thd_lock(thread_mutex);
    s = status.load(std::memory_order_relaxed);
    if (CheckIfAwake(s)) {
      return std::cv_status::no_timeout;
    }
    status.store(kStatusPending, std::memory_order_release);

    if (timeout < 0) {
      thread_cv.wait(thd_lock);
    } else {
      thread_cv.wait_for(thd_lock, std::chrono::milliseconds(timeout));
    }
    return notify_lock.try_lock() ? std::cv_status::timeout
                                  : std::cv_status::no_timeout;
  }

  virtual bool notify_one() override {
    std::unique_lock<std::mutex> thd_lock(thread_mutex);
    if (!notify_lock.try_lock()) {
      return false;
    }
    status.store(kStatusAwake, std::memory_order_release);
    thread_cv.notify_one();
    return true;
  }
} LIBCO_CACHELINE_ALIGNMENT;

}  // namespace detail
}  // namespace libco
