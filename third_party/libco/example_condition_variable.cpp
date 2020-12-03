#include "co_condition_variable.h"
#include "co_routine_lambda.h"

#include <deque>
#include <mutex>
#include <thread>

template <typename T>
class CoBlockingQueue {
 public:
  CoBlockingQueue(int capacity) : capacity_(capacity) {}

  ~CoBlockingQueue() = default;

  void Push(const T& t) {
    std::unique_lock<std::mutex> ul(mutex_);
    while (queue_.size() >= capacity_) {
      in_cv_.wait(ul);
    }
    queue_.emplace_back(t);
    out_cv_.notify_one();
  }

  void Pop(T* t) {
    std::unique_lock<std::mutex> ul(mutex_);
    while (queue_.size() <= 0) {
      out_cv_.wait(ul);
    }
    *t = queue_.front();
    queue_.pop_front();
    in_cv_.notify_one();
  }

  size_t Size() {
    std::unique_lock<std::mutex> ul(mutex_);
    return queue_.size();
  }

 private:
  std::deque<T> queue_;
  int capacity_;

  std::mutex mutex_;
  libco::ConditionVariable in_cv_;
  libco::ConditionVariable out_cv_;
};

CoBlockingQueue<int> blocking_queue(10);

void Producer() {
  for (int i = 0; i < 5; ++i) {
    co_create([]() {
      co_enable_hook_sys();
      int number = 0;
      while (true) {
        blocking_queue.Push(number++);
      }
    });
  }

  co_eventloop(co_get_epoll_ct(), nullptr);
}

void Consumer() {
  for (int i = 0; i < 5; ++i) {
    co_create([]() {
      co_enable_hook_sys();
      while (true) {
        int number = 0;
        blocking_queue.Pop(&number);
        printf("Pop number %d\n", number);
        poll(nullptr, 0, 10);
      }
    });
  }

  co_eventloop(co_get_epoll_ct(), nullptr);
}

int main(int argc, char* argv[]) {
  std::vector<std::thread> thread_list;

  thread_list.emplace_back(std::thread(&Producer));
  for (int i = 0; i < 5; ++i) {
    thread_list.emplace_back(std::thread(&Consumer));
  }

  for (auto& t : thread_list) {
    t.join();
  }

  return 0;
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

