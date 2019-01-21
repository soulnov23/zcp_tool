#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <iostream>
using namespace std;

/*
#define PRINTF(format, ...) \
		printf("%s : %s : %d"format"%s", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__, strerror(errno))
*/
#define PRINTF_DEBUG(format, ...) \
		printf("[%s %s] [DEBUG] %s:%d %s():"format"\n", __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define PRINTF_ERROR(format, ...) \
		printf("[%s %s] [ERROR]%s:%d: %s():%s"format"\n", __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION__, strerror(errno), ##__VA_ARGS__)

template <typename T1, typename T2, typename T3>
void PRINTF_MAP(map<T1, T2> &record, T3 &it)
{
	it = record.begin();
	while (it != record.end())
	{
		cout << it->first << ":" << it->second << endl;
		it++;
	}
}

#endif