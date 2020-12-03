#pragma once

#include "macros.h"

#include <memory>
#include <atomic>
#include <cstdint>

namespace libco {

class SpinLock {
 public:
  SpinLock() : lock_(FREE){};

  bool try_lock() noexcept { return cas(FREE, LOCKED); }

  void lock() noexcept {
    while (!try_lock()) {
      while (lock_.load(std::memory_order_relaxed) == LOCKED) {
        asm_volatile_pause();
      }
    }
  }

  void unlock() noexcept { lock_.store(FREE, std::memory_order_release); }

  LIBCO_NONCOPYABLE(SpinLock);

 private:
  enum { FREE = 0, LOCKED = 1 };

  std::atomic<uint8_t> lock_;

  bool cas(uint8_t compare, uint8_t new_val) noexcept {
    return lock_.compare_exchange_strong(
        compare, new_val, std::memory_order_acquire, std::memory_order_relaxed);
  }
};

}  // namespace libco
