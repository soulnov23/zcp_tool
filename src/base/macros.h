#pragma once

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
