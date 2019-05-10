#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#include <string>
using namespace std;

int get_time_now(string &str_now);
//字符串时间格式2019-05-09 11:17:55
time_t str_time2date(const string &str_time);
string date2str_time(time_t time);

#endif