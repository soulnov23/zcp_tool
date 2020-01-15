#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#include <string>
using namespace std;

//字符串时间格式2019-05-09 11:17:55 175827
int get_time_now(string& str_now);
//获取从1970-01-01 00:00:00 +0000(UTC)到现在的时间，精度秒
time_t get_time_sec();
//获取从1970-01-01 00:00:00 +0000(UTC)到现在的时间，精度毫秒
time_t get_time_usec();
//获取时区
int get_time_zone(int& time_zone);
//字符串时间转time_t时间
time_t str_time2date(const string& str_time);
//time_t时间转字符串时间
string date2str_time(time_t time);

#endif