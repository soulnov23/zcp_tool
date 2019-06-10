#ifndef __AIO_LIB_UTILS_H__
#define __AIO_LIB_UTILS_H__

#include <stack>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class PipeAlloc_st {
  std::stack<int *> v_;

 public:
  int *alloc() {
    if (!v_.empty()) {
      int *ret = v_.top();
      v_.pop();
      return ret;
    }
    int *p = new int[2];
    int ret = CreatePipe(p, 1024);
    if (ret) return 0;

    return p;
  }
  void free(int *i) {
    if (i) {
      v_.push(i);
    }
  }
  static int CreatePipe(int fd[2], int bufsize) {
    int ret = pipe(fd);
    if (ret != 0) return ret;

    int f = fcntl(fd[1], F_GETFL, 0);
    fcntl(fd[1], F_SETFL, f | O_NONBLOCK);
    f = fcntl(fd[0], F_GETFL, 0);
    fcntl(fd[0], F_SETFL, f | O_NONBLOCK);

    return 0;
  }
};

class Pos {
 public:
  Pos() : off(0), len(0) {}
  Pos(off_t o, size_t l) : off(o), len(l) {}

  bool operator==(const Pos &other) const {
    return off == other.off && len == other.len;
  }

  off_t off;
  size_t len;

 public:
  static Pos GetAlignPos(const Pos &raw, const size_t alignment) {
    Pos ret;

    off_t align_begin = raw.off / alignment * alignment;
    off_t align_end = raw.off + raw.len;

    if (align_end % alignment != 0) {
      align_end = (align_end / alignment + 1) * alignment;
    }

    ret.off = align_begin;
    ret.len = align_end - align_begin;

    return ret;
  }
  static Pos GetIntersesion(const Pos &a, const Pos &b) {
    Pos ret;
    if ((a.off > b.off && a.off - b.off > (off_t)b.len) ||
        (b.off > a.off && b.off - a.off > (off_t)a.len)) {
      return ret;
    }
    ret.off = std::max(a.off, b.off);
    ret.len = std::min(a.off + a.len, b.off + b.len) - ret.off;
    return ret;
  }
};

Pos GetAlignPos(const Pos &raw, const size_t alignment);
Pos GetIntersesion(const Pos &a, const Pos &b);
template <class T>
class clsLinkedList_mt {
  volatile T *head_;

 public:
  clsLinkedList_mt() : head_(0) {}
  ~clsLinkedList_mt() {}

  bool push_front(volatile T *apNode, int aiRetry) {
    bool ret = false;
    for (int i = 0; i < aiRetry; i++) {
      volatile T *lpOldHead = head_;
      apNode->pHeadNext = head_;
      ret = __sync_bool_compare_and_swap(&head_, lpOldHead, apNode);
      if (ret) {
        break;
      }
    }
    return ret;
  }
  volatile T *take_all() {
    volatile T *lpOldHead = NULL;
    for (;;) {
      lpOldHead = head_;
      int ret = __sync_bool_compare_and_swap(&head_, lpOldHead, NULL);
      if (ret) {
        break;
      }
    }
    return lpOldHead;
  }
  volatile T *take_all_reverse() {
    volatile T *p = (T *)take_all();
    volatile T *nil = 0;
    while (p) {
      volatile T *q = p->pHeadNext;
      p->pHeadNext = nil;
      nil = p;
      p = q;
    }
    return nil;
  }
};
template <class T>
class clsPtrQueue {
  T *head_;
  T *tail_;

 public:
  clsPtrQueue() : head_(0), tail_(0) {}
  ~clsPtrQueue() {}
  void push(T *p) {
    if (p->pHeadNext) p->pHeadNext = 0;
    join(p);
  }
  void join(T *p) {
    if (!head_) {
      head_ = p;
      tail_ = p;
      return;
    }
    tail_->pHeadNext = p;
    tail_ = p;
  }
  void pop() {
    head_ = head_->pHeadNext;
    if (!head_) tail_ = 0;
  }
  bool empty() const { return !head_; }
  T *top() const { return head_; }
  void clear() {
    head_ = 0;
    tail_ = 0;
  }
};
#endif
