#ifndef __TOOL_H__
#define __TOOL_H__

#include <map>
#include <iostream>
#include <string>
using namespace std;

typedef map<string, string> Data;

template <typename T1, typename T2, typename T3>
void PRINTF_MAP(map<T1, T2> &record, T3 &it);

void map2str(string &url, const Data &data);
void str2map(string &buf, Data &data);

#endif