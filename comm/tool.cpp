#include "tool.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define FIELD_FLAG		"&"
#define VALUE_FLAG		"="

#define HEADER_FLAG		"\r\n"
#define HEADER_VALUE	": "

static int get_field(string &str, string &value)
{
	if(str.size() ==0)
	{	
		return 0;
	}

	string::size_type start = str.find(FIELD_FLAG);
	if(start == string::npos || start !=0)
	{
		start= 0;
	}
	else
	{
		start += strlen(FIELD_FLAG);
	}

	string::size_type end = str.find(FIELD_FLAG, start);
	if(end == string::npos)
	{
		end = str.size();
	}

	value = str.substr(start, end - start);
	str = str.substr(end);

	return 1;
}
 
static int get_value(const string &str, string &name, string &value)
{
	string::size_type pos = str.find(VALUE_FLAG);
	if(pos == string::npos)
	{
		return 0;
	}

	name = str.substr(0, pos);
	value = str.substr(pos + strlen(VALUE_FLAG));

	return 1;
}


void map2str(string &url, const record_t &record)
{
	for(record_t::const_iterator it = record.begin(); it != record.end(); it++)
	{
		url += it->first.c_str();
		url += VALUE_FLAG;
		url += it->second.c_str();	
		url += FIELD_FLAG;
	}

	if(url.size())
	{
		string tmp(url.c_str(), url.size()-1);
		url = tmp;
	}
}

void map2header(string &header, const record_t &record)
{
	for(record_t::const_iterator it = record.begin(); it != record.end(); it++)
	{
		header += it->first.c_str();
		header += HEADER_VALUE;
		header += it->second.c_str();	
		header += HEADER_FLAG;
	}
}

void str2map(const string &buf, record_t &record)
{
	string bvalue, fname, fvalue;

	string stbuf = buf;
	while(get_field(stbuf, bvalue))
	{
		if(!get_value(bvalue, fname, fvalue))
		{
			continue;
		}
		record[fname]=fvalue; 
	}
}

void str2vec(const string &buf, const string &field, vector_t &vec)
{
	vec.clear();
	size_t offset = 0;
	size_t next = 0;
	while (true)
	{
		next = buf.find_first_of(field, offset);
		if (next == string::npos)
		{
			vec.push_back(buf.substr(offset));
			break;
		}
		vec.push_back(buf.substr(offset, next-offset));
		offset = next + 1;
	}
}

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

void string_replace(string &data, const string &src, const string &dst)
{
	string::size_type pos=0;
	string::size_type srclen=src.size();
	string::size_type dstlen=dst.size();
	while((pos=data.find(src, pos)) != string::npos)
	{
		data.replace(pos, srclen, dst);
		pos += dstlen;
	}
}

void string_lower(string &str)
{
	for (string::iterator i = str.begin(); i != str.end(); i++)
	{
		*i = tolower(*i);		
	}
}

void string_upper(string &str)
{
	for (string::iterator i = str.begin(); i != str.end(); i++)
	{
		*i = toupper(*i);		
	}
}

const string longlong_to_string(long long ll)
{
    char tmp[128] = {0};
    snprintf(tmp, sizeof(tmp)-1, "%lld", ll);
    return tmp;
}