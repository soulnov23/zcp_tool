#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "src/base/time_util.h"

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
#define PRINTF_DEBUG(format, args...)                                                                      \
    do {                                                                                                   \
        printf("[%s] [" GREEN_PRINT_BEG "debug" GREEN_PRINT_END "] [process %d][%s:%d %s()] " format "\n", \
               get_time_now().c_str(), getpid(), __FILE__, __LINE__, __FUNCTION__, ##args);                \
        fflush(stdout);                                                                                    \
    } while (0)

#define PRINTF_ERROR(format, args...)                                                                                    \
    do {                                                                                                                 \
        printf("[%s] [" RED_PRINT_BEG "error" RED_PRINT_END "] [process %d][%s:%d %s()] [errno:%d err:%s] " format "\n", \
               get_time_now().c_str(), getpid(), __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno), ##args);      \
        fflush(stdout);                                                                                                  \
    } while (0)
*/

#define PRINTF_DEBUG(format, ...)                                                                          \
    do {                                                                                                   \
        printf("[%s] [" GREEN_PRINT_BEG "debug" GREEN_PRINT_END "] [process %d][%s:%d %s()] " format "\n", \
               get_time_now().c_str(), getpid(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);         \
        fflush(stdout);                                                                                    \
    } while (0)

#define PRINTF_ERROR(format, ...)                                                                                          \
    do {                                                                                                                   \
        printf("[%s] [" RED_PRINT_BEG "error" RED_PRINT_END "] [process %d][%s:%d %s()] [errno:%d err:%s] " format "\n",   \
               get_time_now().c_str(), getpid(), __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno), ##__VA_ARGS__); \
        fflush(stdout);                                                                                                    \
    } while (0)
