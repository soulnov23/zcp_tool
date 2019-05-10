#ifndef __TOOL_H__
#define __TOOL_H__

#include <string>
#include <vector>
#include <string>
#include <map>
using namespace std;

int get_time_now(string &str_now);
//字符串时间格式2019-05-09 11:17:55
time_t str_time2date(const string &str_time);
string date2str_time(time_t time);

void string_replace(string &data, const string &src, const string &dst);
void string_lower(string &str);
void string_upper(string &str);

const string longlong_to_string(long long ll);

typedef vector<string> vector_t;
void str2vec(const string &buf, const string &field, vector_t &vec);

typedef map<string, string> record_t;
void map2str(string &buf, const record_t &record, bool encode = true);
void str2map(record_t &record, const string &buf, bool encode = true);

#endif