#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define PRINTF_DEBUG(format, ...) \
		printf("[%s %s] [DEBUG] %s:%d %s() "format"\n", __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define PRINTF_ERROR(format, ...) \
		printf("[%s %s] [ERROR] %s:%d %s() "format"\n", __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
		printf("[%s %s] [ERROR] %s:%d %s() errno:%d err:%s\n", __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION__, \ 
		errno, strerror(errno))

#endif