#include "tool.h"
#include <string.h>

#define FIELD_FLAG     "&"    
#define VALUE_FLAG     "=" 

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


void map2str(string &url, const Data &data)
{
    for(Data::iterator it = data.begin(); it != data.end(); it++)
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

void str2map(string &buf, Data &data)
{
    string bvalue, fname, fvalue;
 
    string stbuf = buf;
    while(get_field(stbuf, bvalue))
    {
		if(!get_value(bvalue, fname, fvalue))
		{
			continue;
		}
		data[fname]=fvalue; 
    }
}