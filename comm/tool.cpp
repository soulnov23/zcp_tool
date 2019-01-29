#include "tool.h"
#include <string.h>

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

void str2map(string &buf, record_t &record)
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

string lower(const string &src)
{
    string dst;
    for(size_t i=0; i<src.length(); i++) 
	{
        dst += ((unsigned char)tolower(src.c_str()[i]));
    }
    return dst;
}

string upper(const string &src)
{
    string dst;
    for(size_t i=0; i<src.length(); i++) 
	{
        dst += ((unsigned char)toupper(src.c_str()[i]));
    }
    return dst;
}