#pragma once

#include <cstddef>
#include <new>

// 单例的析构策略
enum class singleton_policy {
    // 退出时按构造的逆序析构，默认策略
    destroy,
    // 永不析构，适用于必须活到其它静态对象析构之后的单例（如日志）。
    // 此类单例需提供显式的收尾接口，用来刷盘、回收线程等
    no_destroy,
};

// CRTP单例基类。
// 线程安全由C++11起的magic static规则保证（[stmt.dcl]/4）：函数局部static的初始化
// 线程安全，并发到达时其它线程阻塞等待，不需要手写double-checked locking。
// 派生类须将构造和析构函数声明为private，并声明friend class singleton<派生类[, 策略]>
template <typename T, singleton_policy policy = singleton_policy::destroy>
class singleton {
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;

public:
    static T& instance() {
        if constexpr (policy == singleton_policy::no_destroy) {
            // 在静态存储上原地构造且不注册析构，规避退出期的析构顺序问题
            alignas(T) static std::byte storage[sizeof(T)];
            static T* inst = new (&storage) T;
            return *inst;
        } else {
            static T inst;
            return inst;
        }
    }

protected:
    // 非virtual：CRTP基类不作多态使用，virtual会给每个单例引入无用的vptr
    singleton() = default;
    ~singleton() = default;
};
