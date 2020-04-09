#pragma once

#include "co_mutex.h"
#include "co_condition_variable.h"

namespace libco {

class CoRWMutex {
 public:
  CoRWMutex() : state_(CoRWMutex::IDLE) {}

  void ReadLock() {
    uint8_t s = AtomicState()->load(std::memory_order_relaxed);
    if (s != CoRWMutex::WRITE) {
      if (AtomicState()->compare_exchange_weak(s, s + 1)) {
        return;
      }
    }

    std::unique_lock<std::mutex> lock(mutex_);
    if (state_ != CoRWMutex::WRITE) {
      ++state_;
      return;
    }
    cv_.wait(lock);
  }

  void WriteLock() {
    if (AtomicState()->compare_exchange_weak(CoRWMutex::IDLE,
                                             CoRWMutex::WRITE)) {
      return;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    if (state_ == CoRWMutex::IDLE) {
      state_ = CoRWMutex::WRITE;
      return;
    }
    cv_.wait(lock);
  }

  void UnLock() {
    uint8_t s = AtomicState()->load(std::memory_order_relaxed);
    if (s == CoRWMutex::IDLE) {
      return;
    }

    if (s > CoRWMutex::IDLE) {
      if (AtomicState()->compare_exchange_weak(s, s - 1)) {
        return;
      }
    }

    if (AtomicState()->compare_exchange_weak(CoRWMutex::IDLE,
                                             CoRWMutex::WRITE)) {
      return;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_.notify_one()) {
      if (state_ > CoRWMutex::IDLE) {
        --state_;
        return;
      }

      state_ == CoRWMutex::IDLE;
      return;
    }
  }

 private:
  enum RWLockState : uint8_t {
    WRITE = 1,
    IDLE = 2,
    // Use state > 2 to represent read
  };

  std::mutex mutex_;
  ConditionVariable cv_;

  uint8_t state_;

  std::atomic<uint8_t>* AtomicState() {
    return reinterpret_cast<std::atomic<uint8_t>*>(&this->state_);
  }
};

}  // namespace libco
