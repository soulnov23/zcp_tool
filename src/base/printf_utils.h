#ifndef __PRINTF_UTILS_H__
#define __PRINTF_UTILS_H__

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "src/base/time_utils.h"

/*
格式：\033[显示方式;前景色;背景色m
\e等同于\033 0m取消颜色设置
*/
#define RED_PRINT_BEG    "\e[1;31m"
#define RED_PRINT_END    "\e[m"
#define GREEN_PRINT_BEG  "\e[1;32m"
#define GREEN_PRINT_END  "\e[m"
#define YELLOW_PRINT_BEG "\e[1;33m"
#define YELLOW_PRINT_END "\e[m"
#define BLUE_PRINT_BEG   "\e[1;34m"
#define BLUE_PRINT_END   "\e[m"

/*
gcc支持的做法，支持arg可变参传入
#define PRINTF_DEBUG(format, args...) \
                printf("DEBUG [%s:%d %s()] " format"\n", __FILE__, __LINE__,
__PRETTY_FUNCTION__, ##args)
#define PRINTF_DEBUG(format, args...) \
                printf("DEBUG [%s:%d %s()] " format"\n", __FILE__, __LINE__,
__PRETTY_FUNCTION__, ##args)
*/

#define PRINTF_DEBUG(format, ...)                                                                                        \
    {                                                                                                                    \
        printf("[%s] " GREEN_PRINT_BEG "DEBUG" GREEN_PRINT_END " [%d][%s:%d %s()] " format "\n", get_time_now().c_str(), \
               getpid(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                                               \
        fflush(stdout);                                                                                                  \
    }

#define PRINTF_ERROR(format, ...)                                                                                          \
    {                                                                                                                      \
        printf("[%s] " RED_PRINT_BEG "ERROR" RED_PRINT_END " [%d][%s:%d %s()] [errno:%d err:%s] " format "\n",             \
               get_time_now().c_str(), getpid(), __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno), ##__VA_ARGS__); \
        fflush(stdout);                                                                                                    \
    }

#define RETURN_ON_ERROR(expr)                \
    do {                                     \
        int __ret = (expr);                  \
        if (__ret != 0) {                    \
            PRINTF_ERROR("call expr error"); \
            return __ret;                    \
        }                                    \
    } while (0)

#define RETURN_ON_ERROR_OR_DO(expr, sucess_expr) \
    do {                                         \
        int __ret = (expr);                      \
        if (__ret != 0) {                        \
            PRINTF_ERROR("call expr error");     \
            return __ret;                        \
        }                                        \
        sucess_expr;                             \
    } while (0)

#define ON_ERROR_RETURN(expr, ret_err)           \
    do {                                         \
        if ((expr) != 0)                         \
            if (__ret != 0) {                    \
                PRINTF_ERROR("call expr error"); \
                return (ret_err);                \
            }                                    \
    } while (0)

#endif
