#pragma once

#include <atomic>
#include <memory>

#include "src/base/macros.h"

enum status {
    LOCKFREE_QUEUE_OK = 0,    // 操作成功
    LOCKFREE_QUEUE_ERR = -1,  // 操作失败
    LOCKFREE_QUEUE_FULL = 1,  // 入队列失败，队列已满
    LOCKFREE_QUEUE_EMPTY = 2  // 出队列失败，队列为空
};

template <typename T>
class lockfree_queue_t {
private:
    CLASS_UNCOPYABLE(lockfree_queue_t)
    CLASS_UNMOVABLE(lockfree_queue_t)

    void init(std::size_t size) {
        // 判断队列长度必须是2的幂，以便使用版本号和mask_&位操作，保证nodes的索引一直都在size-1范围内
        bool size_is_power_of_2 = (size >= 2) && ((size & (size - 1)) == 0);
        if (!size_is_power_of_2) {
            size = g_default_size;
        }
        nodes_ = std::make_unique<node_t[]>(size);
        for (std::size_t i = 0; i < size; i++) {
            nodes_[i].seq_.store(i, std::memory_order_relaxed);
        }
        size_ = size;
        mask_ = size - 1;
        capacity_.store(0, std::memory_order_relaxed);
        enqueue_seq_.store(0, std::memory_order_relaxed);
        dequeue_seq_.store(0, std::memory_order_relaxed);
    }

public:
    lockfree_queue_t() { init(g_default_size); }
    lockfree_queue_t(std::size_t size) { init(size); }
    ~lockfree_queue_t() {}
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_.load(std::memory_order_acquire); }

    int enqueue(const T& data) {
        node_t* node = nullptr;
        std::size_t enqueue_seq = enqueue_seq_.load(std::memory_order_relaxed);
        while (true) {
            node = &nodes_[enqueue_seq & mask_];
            std::size_t node_seq = node->seq_.load(std::memory_order_acquire);
            long dif = node_seq - enqueue_seq;
            if (dif == 0) {
                if (enqueue_seq_.compare_exchange_weak(enqueue_seq, enqueue_seq + 1, std::memory_order_relaxed)) {
                    break;
                }
            } else if (dif < 0) {
                if (enqueue_seq - dequeue_seq_.load(std::memory_order_relaxed) == size_) {
                    return LOCKFREE_QUEUE_FULL;
                }
                enqueue_seq = enqueue_seq_.load(std::memory_order_relaxed);
            } else {
                enqueue_seq = enqueue_seq_.load(std::memory_order_relaxed);
            }
        }
        node->data_ = data;
        node->seq_.store(enqueue_seq + 1, std::memory_order_acquire);
        capacity_.fetch_add(1, std::memory_order_acquire);
        return LOCKFREE_QUEUE_OK;
    }

    int dequeue(T& data) {
        node_t* node = nullptr;
        std::size_t dequeue_seq = dequeue_seq_.load(std::memory_order_relaxed);
        while (true) {
            node = &nodes_[dequeue_seq & mask_];
            std::size_t node_seq = node->seq_.load(std::memory_order_acquire);
            long dif = node_seq - (dequeue_seq + 1);
            if (dif == 0) {
                if (dequeue_seq_.compare_exchange_weak(dequeue_seq, dequeue_seq + 1, std::memory_order_relaxed)) {
                    break;
                }
            } else if (dif < 0) {
                if (dequeue_seq - enqueue_seq_.load(std::memory_order_relaxed) == 0) {
                    return LOCKFREE_QUEUE_EMPTY;
                }
                dequeue_seq = dequeue_seq_.load(std::memory_order_relaxed);
            } else {
                dequeue_seq = dequeue_seq_.load(std::memory_order_relaxed);
            }
        }
        data = node->data_;
        node->seq_.store(dequeue_seq + 1 + mask_, std::memory_order_acquire);
        capacity_.fetch_sub(1, std::memory_order_acquire);
        return LOCKFREE_QUEUE_OK;
    }

private:
    static constexpr std::size_t hardware_destructive_interference_size = 128;  // 硬件破坏性干扰大小，内存对齐
    struct alignas(hardware_destructive_interference_size) node_t {
        T data_;
        // seq_在线程1的load操作Synchronizes-with在线程2的store操作，原子操作的内存顺序都是memory_order_acquire和memory_order_release配合使用
        std::atomic<std::size_t> seq_;
    };

private:
    static constexpr std::size_t g_default_size = 128;  // queue默认大小
    std::size_t size_;                                  // queue初始化大小
    std::size_t mask_;                                  // size-1
    std::atomic<std::size_t> capacity_;                 // queue实际存储node大小
    std::unique_ptr<node_t[]> nodes_;
    // enqueue_seq_和dequeue_seq_对于其它读写操作没有任何同步和重排的限制，仅要求保证原子性和内存一致性，所有原子操作的内存顺序都使用memory_order_relaxed
    // enqueue_seq_和dequeue_seq_同时作为版本号，一直递增防止ABA问题
    alignas(hardware_destructive_interference_size) std::atomic<std::size_t> enqueue_seq_;
    alignas(hardware_destructive_interference_size) std::atomic<std::size_t> dequeue_seq_;
};