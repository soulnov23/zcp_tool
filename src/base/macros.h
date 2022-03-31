#pragma once

#include <errno.h>

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

#define RETURN_ON_ERROR(expr) \
    do {                      \
        int __ret = (expr);   \
        if (__ret != 0) {     \
            return __ret;     \
        }                     \
    } while (0)

#define HANDLE_EINTR(x)                                                                          \
    ({                                                                                           \
        int eintr_wrapper_counter = 0;                                                           \
        decltype(x) eintr_wrapper_result;                                                        \
        do {                                                                                     \
            eintr_wrapper_result = (x);                                                          \
        } while (eintr_wrapper_result == -1 && errno == EINTR && eintr_wrapper_counter++ < 100); \
        eintr_wrapper_result;                                                                    \
    })

#define IGNORE_EINTR(x)                                         \
    ({                                                          \
        decltype(x) eintr_wrapper_result;                       \
        do {                                                    \
            eintr_wrapper_result = (x);                         \
            if (eintr_wrapper_result == -1 && errno == EINTR) { \
                eintr_wrapper_result = 0;                       \
            }                                                   \
        } while (0);                                            \
        eintr_wrapper_result;                                   \
    })
