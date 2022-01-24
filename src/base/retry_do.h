#pragma once

#include <errno.h>
#include <sys/types.h>

// Wrap call to func(args...) in loop to retry on EINTR
template <typename function_t, typename... args_t>
int retry_do(function_t func, args_t... args) {
    ssize_t n;
    do {
        n = func(args...);
    } while (n == -1 && errno == EINTR);  // EINTR时重试
    return n;
}