#include "co_mutex.h"

namespace libco {

void CoMutex::lock() {
  if (--tickets_ == 0) {
    return;
  }

  std::unique_lock<std::mutex> lock(mutex_);
  if (notified_) {
    notified_ = false;
    return;
  }
  cv_.wait(lock);
}

bool CoMutex::try_lock() {
  if (--tickets_ == 0) {
    return true;
  }

  ++tickets_;
  return false;
}

void CoMutex::unlock() {
  if (++tickets_ == 1) {
    return;
  }

  std::unique_lock<std::mutex> lock(mutex_);
  if (!cv_.notify_one()) {
    notified_ = true;
  }
}

}  // namespace libco

//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

