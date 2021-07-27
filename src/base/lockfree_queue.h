#ifndef __LOCKFREE_QUEUE_H__
#define __LOCKFREE_QUEUE_H__

#include <atomic>
#include <memory>
using namespace std;

enum status {
    LOCKFREE_QUEUE_OK    = 0,   // 操作成功
    LOCKFREE_QUEUE_ERR   = -1,  // 操作失败
    LOCKFREE_QUEUE_FULL  = 1,   // 入队列失败，队列已满
    LOCKFREE_QUEUE_EMPTY = 2    // 出队列失败，队列为空
};

template <typename T>
class lockfree_queue_t {
private:
    void init(size_t size) {
        bool size_is_power_of_2 = (size >= 2) && ((size & (size - 1)) == 0);
        if (!size_is_power_of_2) {
            size = g_default_size;
        }
        nodes_ = make_unique<node_t[]>(size);
        for (size_t i = 0; i < size; i++) {
            nodes_[i].seq_ = i;
        }
        size_ = size;
        mask_ = size - 1;
        capacity_.store(0, memory_order_relaxed);
        enqueue_seq_.store(0, memory_order_relaxed);
        dequeue_seq_.store(0, memory_order_relaxed);
    }
    lockfree_queue_t(lockfree_queue_t&& rhs)      = delete;
    lockfree_queue_t(const lockfree_queue_t& rhs) = delete;

    lockfree_queue_t& operator=(lockfree_queue_t&& rhs) = delete;
    lockfree_queue_t& operator=(const lockfree_queue_t& rhs) = delete;

public:
    lockfree_queue_t() { init(g_default_size); }
    lockfree_queue_t(size_t size) { init(size); }
    ~lockfree_queue_t() {}
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_.load(memory_order_relaxed); }

    int enqueue(const T& data) {
        node_t* node       = nullptr;
        size_t enqueue_seq = enqueue_seq_.load(memory_order_relaxed);
        while (true) {
            node            = &nodes_[enqueue_seq & mask_];
            size_t node_seq = node->seq_.load(memory_order_acquire);
            long dif        = node_seq - enqueue_seq;
            if (dif == 0) {
                if (enqueue_seq_.compare_exchange_weak(enqueue_seq, enqueue_seq + 1, memory_order_relaxed)) {
                    break;
                }
            } else if (dif < 0) {
                if (enqueue_seq - dequeue_seq_.load(memory_order_relaxed) == size_) {
                    return LOCKFREE_QUEUE_FULL;
                }
                enqueue_seq = enqueue_seq_.load(memory_order_relaxed);
            } else {
                enqueue_seq = enqueue_seq_.load(memory_order_relaxed);
            }
        }
        node->data_ = data;
        node->seq_.store(enqueue_seq + 1, memory_order_release);
        capacity_.fetch_add(1, memory_order_release);
        return LOCKFREE_QUEUE_OK;
    }

    int dequeue(T& data) {
        node_t* node       = nullptr;
        size_t dequeue_seq = dequeue_seq_.load(memory_order_relaxed);
        while (true) {
            node            = &nodes_[dequeue_seq & mask_];
            size_t node_seq = node->seq_.load(memory_order_acquire);
            long dif        = node_seq - (dequeue_seq + 1);
            if (dif == 0) {
                if (dequeue_seq_.compare_exchange_weak(dequeue_seq, dequeue_seq + 1, memory_order_relaxed)) {
                    break;
                }
            } else if (dif < 0) {
                if (dequeue_seq - enqueue_seq_.load(memory_order_relaxed) == 0) {
                    return LOCKFREE_QUEUE_EMPTY;
                }
                dequeue_seq = dequeue_seq_.load(memory_order_relaxed);
            } else {
                dequeue_seq = dequeue_seq_.load(memory_order_relaxed);
            }
        }
        data = node->data_;
        node->seq_.store(dequeue_seq + 1 + mask_, memory_order_release);
        capacity_.fetch_sub(1, memory_order_release);
        return LOCKFREE_QUEUE_OK;
    }

private:
    static constexpr size_t hardware_destructive_interference_size = 128;  // 硬件破坏性干扰大小，内存对齐
    struct alignas(hardware_destructive_interference_size) node_t {
        T data_;
        atomic<size_t> seq_;
    };

private:
    static constexpr size_t g_default_size = 128;  // queue默认大小
    size_t size_;                                  // queue初始化大小
    size_t mask_;                                  // size-1
    atomic<size_t> capacity_;                      // queue实际存储node大小
    unique_ptr<node_t[]> nodes_;
    alignas(hardware_destructive_interference_size) atomic<size_t> enqueue_seq_;
    alignas(hardware_destructive_interference_size) atomic<size_t> dequeue_seq_;
};

#endif