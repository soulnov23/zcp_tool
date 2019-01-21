#ifndef __STL_TOOL_H__
#define __STL_TOOL_H__

#include <map>
#include <iostream>
using namespace std;

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