#include "tool.h"
#include <string.h>

#define FIELD_FLAG     "&"    
#define VALUE_FLAG     "="
 
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