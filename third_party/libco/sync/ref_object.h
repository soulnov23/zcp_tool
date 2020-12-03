#pragma once
#include "macros.h"

#include <atomic>
#include <type_traits>

namespace libco {

template <typename T>
class AtomicRefObject {
 public:
  AtomicRefObject() : ref_(0) {}

  virtual ~AtomicRefObject() = default;

  int Ref() { return ref_.fetch_add(1); }

  int Unref() {
    int old_ref = ref_.fetch_sub(1);
    if (old_ref == 1) {
      delete static_cast<const T*>(this);
    }
    return old_ref;
  }

  LIBCO_NONCOPYABLE(AtomicRefObject);

 protected:
  std::atomic<int> ref_;
};

template <typename T, typename = typename std::enable_if<
                          std::is_base_of<AtomicRefObject<T>, T>::value>::type>
class AutoFreeHelper {
 public:
  AutoFreeHelper(T* ptr) : ptr_(ptr) {}

  ~AutoFreeHelper() {
    if (ptr_ != nullptr) {
      ptr_->Unref();
    }
  }

  LIBCO_NONCOPYABLE(AutoFreeHelper);

 private:
  T* ptr_;
};

}  // namespace libco
