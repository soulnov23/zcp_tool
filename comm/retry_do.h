#ifndef __RETRY_DO_H__
#define __RETRY_DO_H__

#include <errno.h>
#include <sys/types.h>

#define r_do4(func, p1, p2, p3, p4) ({ \
    ssize_t ret_ = func(p1, p2, p3, p4); \
    while (ret_ == -1 && errno == EINTR) \
        ret_ = func(p1, p2, p3, p4); \
    ret_; })

#define r_do3(func, p1, p2, p3) ({ \
    ssize_t ret_ = func(p1, p2, p3); \
    while (ret_ == -1 && errno == EINTR) \
        ret_ = func(p1, p2, p3); \
    ret_; })

#define r_do2(func, p1, p2) ({ \
    ssize_t ret_ = func(p1, p2); \
    while (ret_ == -1 && errno == EINTR) \
        ret_ = func(p1, p2); \
    ret_; })

#define r_do1(func, p1) ({ \
    ssize_t ret_ = func(p1); \
    while (ret_ == -1 && errno == EINTR) \
        ret_ = func(p1); \
    ret_; })

#define r_do0(func) ({ \
    ssize_t ret_ = func(); \
    while (ret_ == -1 && errno == EINTR) \
        ret_ = func(); \
    ret_; })


#define r_do  r_do3

///忽略错误运行
#define ignore_error_do(func) \
do{ \
    int err=errno; \
    func; \
    errno = err; \
}while(0)

#define i_do  ignore_error_do

#endif
