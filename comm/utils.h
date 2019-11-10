#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <map>
using namespace std;

//void string_trim(string& str);
void string_replace(string& data, const string& src, const string& dst);
void string_lower(string& str);
void string_upper(string& str);

const string longlong_to_string(long long ll);

typedef vector<string> vector_t;
void str2vec(const string& buf, const string& field, vector_t &vec);

typedef map<string, string> record_t;
void map2str(string& buf, const record_t& record, bool encode = true);
void str2map(record_t &record, const string& buf, bool encode = true);

bool va_function(const char* format, ...);
//bool va_function(const char* format, args...);

#endif