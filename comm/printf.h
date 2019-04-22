#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "tool.h"

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
}

#define PRINTF_ERROR(format, ...) \
{ \
	string time_str; \
	int ret = get_time_now(time_str); \
	if (ret == 0) \
	{ \
		printf("[%s] [ERROR] [%s:%d %s()] [errno:%d err:%s] " format"\n", time_str.c_str(), __FILE__, __LINE__, \
		__FUNCTION__, errno, strerror(errno), ##__VA_ARGS__); \
	} \
	else \
	{ \
		printf("[ERROR] [%s:%d %s()] [errno:%d err:%s] " format"\n", __FILE__, __LINE__, \
		__FUNCTION__, errno, strerror(errno), ##__VA_ARGS__); \
	} \
}

#endif
