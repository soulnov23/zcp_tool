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
public:
    // 下面四个delete的写法要点（条款编号基准C++20/N4868）：
    //
    // 1. 放public而不是private。普通的拷贝/赋值误用下两者诊断相同，都报
    //    "use of deleted function"——重载决议选中deleted函数就已经ill-formed，
    //    访问检查不再额外报错（这是g++/clang的实现行为，标准只要求至少发出一条
    //    诊断，诊断内容是implementation-defined）。但在using声明引入基类成员、
    //    取成员函数地址这类不经重载决议、名字查找直接触发访问检查的语境下，
    //    private会报"is private within this context"，其中using声明还会把
    //    本该ill-formed的代码变成合法（[class.access.general]/4：访问控制对
    //    声明和表达式中引用的名字一视同仁）。单例基类正是会被继承的类型，
    //    放public可避开这一类问题。
    //
    // 2. 移动操作的delete在类型特征层面是冗余的：声明拷贝操作（含= delete）
    //    或声明析构函数都会抑制隐式移动操作的生成（[class.copy.ctor]/8管
    //    移动构造，[class.copy.assign]/4管移动赋值），本类两个条件都占了。
    //    但这里不能省——delete放在public区又省掉移动delete时，派生类可以经
    //    "using singleton::operator=; + 自行声明operator=(const singleton&)"
    //    把基类的右值赋值重新暴露出来，原本ill-formed的代码会变成合法。
    //    要点1和要点2有交互，不能只采纳其一。
    //    （另一种必须显式写出的情况：类里有贪婪的模板构造
    //      template<class U> C(U&&)，模板会赢得重载决议，此时"delete拷贝
    //      即抑制移动"在可观察行为上不成立。本类没有这种构造。）
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
