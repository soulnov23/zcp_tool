#include "time_utils.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

/*localtime非线程安全
int get_time_now(string &str_now)
{
	const string format = "%Y-%m-%d %H:%M:%S";
	time_t t = time(NULL);
	char buf[128];
	struct tm *tm_time = localtime(&t);
	if (!tm_time) 
	{
		return -1;
	}
	strftime(buf, sizeof(buf), format.c_str(), tm_time);
	str_now = buf;
	if (!str_now.length()) 
	{
		return -1;
	}
	return 0;
}
*/

int get_time_now(string &str_now)
{
	struct timeval tv;
	struct tm timestamp;
	if (gettimeofday(&tv, NULL) == -1)
    {
		return -1;
    }
    time_t now = tv.tv_sec;
    if (localtime_r(&now, &timestamp) == NULL)
    {	
		return -1;
    }
	char buf[128];
	if (snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d %06d",
             timestamp.tm_year + 1900, timestamp.tm_mon + 1, timestamp.tm_mday,
             timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, (int)(tv.tv_usec)) < 0)
	{
		return -1;
    }
	str_now = buf;
	return 0;
}

time_t str_time2date(const string &str_time)
{
	struct tm st_time;
	sscanf(str_time.c_str(), 
		   "%04d-%02d-%02d %02d:%02d:%02d",
		   &st_time.tm_year, 
		   &st_time.tm_mon, 
		   &st_time.tm_mday,
		   &st_time.tm_hour, 
		   &st_time.tm_min, 
		   &st_time.tm_sec);
	st_time.tm_year -= 1900;
	st_time.tm_mon -= 1;
	st_time.tm_isdst = 0;
	return mktime(&st_time);
}

string date2str_time(time_t time)
{
	struct tm st_time;
	localtime_r(&time, &st_time);

	char date[30];
	snprintf(date, sizeof(date),
			"%04d-%02d-%02d %02d:%02d:%02d",
			st_time.tm_year + 1900,
			st_time.tm_mon + 1,
			st_time.tm_mday,
			st_time.tm_hour,
			st_time.tm_min,
			st_time.tm_sec);
	return date;
}
