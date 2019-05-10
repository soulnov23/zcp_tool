#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "time_utils.h"

#define RED_PRINT_BEG     "\e[1;31m"
#define RED_PRINT_END     "\e[m"
#define GREEN_PRINT_BEG   "\e[1;32m"
#define GREEN_PRINT_END   "\e[m"
#define YELLOW_PRINT_BEG  "\e[1;33m"
#define YELLOW_PRINT_END  "\e[m"
#define BLUE_PRINT_BEG    "\e[1;34m"
#define BLUE_PRINT_END    "\e[m"

/*
gcc支持的做法，支持arg可变参传入
#define PRINTF_DEBUG(format, args...) \
		printf("[DEBUG] [%s:%d %s()] " format"\n", __FILE__, __LINE__, __FUNCTION__, ##args)
*/

#define PRINTF_DEBUG(format, ...) \
{ \
	string time_str; \
	int ret = get_time_now(time_str); \
	if (ret == 0) \
	{ \
		printf("[%s] [DEBUG] [%s:%d %s()] " format"\n", time_str.c_str(), __FILE__, __LINE__, \
		__FUNCTION__, ##__VA_ARGS__); \
	} \
	else \
	{ \
		printf("[DEBUG] [%s:%d %s()] " format"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
	} \
	fflush(stdout); \
}

#define PRINTF_ERROR(format, ...) \
{ \
	string time_str; \
	int ret = get_time_now(time_str); \
	if (ret == 0) \
	{ \
		printf(RED_PRINT_BEG "[%s] [ERROR] [%s:%d %s()] [errno:%d err:%s] " format"\n" RED_PRINT_END, time_str.c_str(), __FILE__, __LINE__, \
		__FUNCTION__, errno, strerror(errno), ##__VA_ARGS__); \
	} \
	else \
	{ \
		printf(RED_PRINT_BEG "[ERROR] [%s:%d %s()] [errno:%d err:%s] " format"\n" RED_PRINT_END, __FILE__, __LINE__, \
		__FUNCTION__, errno, strerror(errno), ##__VA_ARGS__); \
	} \
	fflush(stdout); \
}

#endif
