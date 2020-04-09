#include "co_mutex.h"
#include "co_routine_lambda.h"

#include <vector>
#include <thread>

static const int kRoutineCnt = 10;
static const int kThreadCnt = 8;
static const int kExecutingCnt = 200;

libco::CoMutex mtx;
static int global_counter = 0;

void CoRoutineCounter() {
  int finished_cnt = 0;
  for (int i = 0; i < kRoutineCnt; ++i) {
    co_create([&finished_cnt]() {
      co_enable_hook_sys();
      for (int k = 0; k < kExecutingCnt; ++k) {
        {
          std::lock_guard<libco::CoMutex> lg(mtx);
          global_counter++;
        }
        poll(nullptr, 0, 5);
      }
      finished_cnt++;
    });
  }

  co_eventloop(co_get_epoll_ct(), [&finished_cnt]() -> int {
    if (finished_cnt == kRoutineCnt) {
      return -1;
    }
    return 0;
  });
}

void ThreadCounter() {
  for (int k = 0; k < kExecutingCnt; ++k) {
    {
      std::lock_guard<libco::CoMutex> lg(mtx);
      global_counter++;
    }
    usleep(1000 * 5);
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::thread> thread_list;

  for (int i = 0; i < kThreadCnt; ++i) {
    if (i % 2 == 0) {
      thread_list.emplace_back(std::thread(&CoRoutineCounter));
    } else {
      thread_list.emplace_back(std::thread(&ThreadCounter));
    }
  }

  for (auto& t : thread_list) {
    t.join();
  }

  printf("global counter %d should == %d\n", global_counter,
         (kThreadCnt / 2 * kExecutingCnt +
          kThreadCnt / 2 * kRoutineCnt * kExecutingCnt));

  return 0;
}

// gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used)) =
    "$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

