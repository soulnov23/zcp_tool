#ifndef __TOOL_H__
#define __TOOL_H__

#include <string>
#include <map>
using namespace std;

typedef map<string, string> record_t;

void map2str(string &url, const record_t &record);
void map2header(string &header, const record_t &record);
void str2map(string &buf, record_t &record);

int get_time_now(string &str_now);

void string_replace(string &data, const string &src, const string &dst);

string lower(const string &src);
string upper(const string &src);

#endif