#pragma once

#include <errno.h>

#include <concepts>
#include <functional>
#include <type_traits>

// 被信号打断的系统调用的重试封装。
// 用函数模板而不是宏：返回类型由调用推导，不会把ssize_t截断成int，
// 也不依赖GNU的语句表达式扩展({...})。
//
// 重载的系统调用（read/write等）需要用lambda包一层来消除歧义：
//     auto n = handle_eintr([&] { return ::read(fd, buf, len); });
// 没有重载的可以直接传函数名：
//     auto ret = handle_eintr(::close, fd);

// 返回值必须是有符号整数才能和 -1 比较。无符号类型下 ret != -1 恒为真，
// EINTR 永远不会重试（比如用在返回 size_t 的函数上）。
// 用signed_integral而不是integral，后者把无符号类型也放进来了
template <typename function_t, typename... args_t>
    requires std::invocable<function_t&, args_t&...> && std::signed_integral<std::invoke_result_t<function_t&, args_t&...>>
[[nodiscard]] auto handle_eintr(function_t&& func, args_t&&... args) -> std::invoke_result_t<function_t, args_t...> {
    // func和args在循环里按左值传递：调用可能重复执行，forward后再次使用会读到被移动的值
    while (true) {
        auto ret = std::invoke(func, args...);
        if (ret != -1 || errno != EINTR) {
            return ret;
        }
    }
}

// EINTR时把-1改写成0表示成功，仅适用于close这类重试本身不安全的调用。
// read/write不要用：返回0在它们的语义里是EOF或者写入0字节
template <typename function_t, typename... args_t>
    requires std::invocable<function_t&, args_t&...> && std::signed_integral<std::invoke_result_t<function_t&, args_t&...>>
[[nodiscard]] auto ignore_eintr(function_t&& func, args_t&&... args) -> std::invoke_result_t<function_t, args_t...> {
    auto ret = std::invoke(std::forward<function_t>(func), std::forward<args_t>(args)...);
    if (ret == -1 && errno == EINTR) {
        ret = 0;
    }
    return ret;
}
