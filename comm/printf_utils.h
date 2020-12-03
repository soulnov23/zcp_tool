#ifndef __PRINTF_UTILS_H__
#define __PRINTF_UTILS_H__

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "zcp_tool/comm/time_utils.h"

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
                printf("[DEBUG] [%s:%d %s()] " format"\n", __FILE__, __LINE__,
__PRETTY_FUNCTION__, ##args)
#define PRINTF_DEBUG(format, args...) \
                printf("[DEBUG] [%s:%d %s()] " format"\n", __FILE__, __LINE__,
__PRETTY_FUNCTION__, ##args)
*/

#define PRINTF_DEBUG(format, ...)                                                                                                \
                                                                                                                                 \
    {                                                                                                                            \
        string __time_str;                                                                                                       \
        int __ret = get_time_now(__time_str);                                                                                    \
        if (__ret == 0) {                                                                                                        \
            printf("[%s] " GREEN_PRINT_BEG "[DEBUG]" GREEN_PRINT_END " [%s:%d %s()] " format "\n", __time_str.c_str(), __FILE__, \
                   __LINE__, __FUNCTION__, ##__VA_ARGS__);                                                                       \
        } else {                                                                                                                 \
            printf(GREEN_PRINT_BEG "[DEBUG]" GREEN_PRINT_END " [%s:%d %s()] " format "\n", __FILE__, __LINE__, __FUNCTION__,     \
                   ##__VA_ARGS__);                                                                                               \
        }                                                                                                                        \
        fflush(stdout);                                                                                                          \
    }

#define PRINTF_ERROR(format, ...)                                                                                            \
                                                                                                                             \
    {                                                                                                                        \
        string __time_str;                                                                                                   \
        int __ret = get_time_now(__time_str);                                                                                \
        if (__ret == 0) {                                                                                                    \
            printf("[%s] " RED_PRINT_BEG "[ERROR]" RED_PRINT_END " [%s:%d %s()] [errno:%d err:%s] " format "\n",             \
                   __time_str.c_str(), __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno), ##__VA_ARGS__);             \
        } else {                                                                                                             \
            printf(RED_PRINT_BEG "[ERROR]" RED_PRINT_END " [%s:%d %s()] [errno:%d err:%s] " format "\n", __FILE__, __LINE__, \
                   __FUNCTION__, errno, strerror(errno), ##__VA_ARGS__);                                                     \
        }                                                                                                                    \
        fflush(stdout);                                                                                                      \
    }

#endif
