#pragma once

#include <atomic>
#include <mutex>

// class uncopyable
#define CLASS_UNCOPYABLE(class_name)        \
private:                                    \
    class_name(const class_name&) = delete; \
    class_name& operator=(const class_name&) = delete;

// class unmovable
#define CLASS_UNMOVABLE(class_name)          \
private:                                     \
    class_name(const class_name&&) = delete; \
    class_name& operator=(const class_name&&) = delete;

template <typename T>
class singleton {
    CLASS_UNCOPYABLE(singleton)
    CLASS_UNMOVABLE(singleton)

    singleton() {}
    ~singleton() {}

public:
    static T* get_instance_atomic() {
        T* tmp = instance_.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            tmp = instance_.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new T;
                instance_.store(tmp, std::memory_order_release);
                atexit(release);
            }
        }
        return tmp;
    }

    static T* get_instance_call_once() {
        std::call_once(once_flag_, [&instance_] { instance_ = new T; });
        atexit(release);
        return instance_;
    }

    static void release() {
        std::lock_guard<std::mutex> lock(mutex_);
        T* tmp = instance_.load(std::memory_order_acquire);
        if (tmp != nullptr) {
            delete tmp;
            instance_.store(nullptr, , std::memory_order_release);
        }
    }

private:
    static std::mutex mutex_;
    static std::atomic<T*> instance_;
    static std::once_flag once_flag_;
};

template <class T>
std::mutex singleton<T>::mutex_;

template <class T>
std::atomic<T*> singleton<T>::instance_{nullptr};

template <class T>
std::once_flag singleton<T>::once_flag_;