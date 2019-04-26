#ifndef __TOOL_H__
#define __TOOL_H__

#include <string>
#include <map>
#include <vector>
using namespace std;

typedef map<string, string> record_t;
typedef vector<string> vector_t;

void map2str(string &url, const record_t &record);
void map2header(string &header, const record_t &record);
void str2map(const string &buf, record_t &record);
void str2vec(const string &buf, const string &field, vector_t &vec);

int get_time_now(string &str_now);

void string_replace(string &data, const string &src, const string &dst);
void string_lower(string &str);
void string_upper(string &str);

const string longlong_to_string(long long ll);


#endif