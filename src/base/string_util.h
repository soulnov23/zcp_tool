#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <map>
#include <string>
#include <vector>
using namespace std;

// void string_trim(string& str);
void string_replace(string& data, const string& src, const string& dst);
void string_replace(string& data, const string& src, size_t num, char character);
void string_lower(string& str);
void string_upper(string& str);

typedef vector<string> vector_t;
void string_split(const string& buf, const char& field, vector_t& vec);
void string_split(const string& buf, const string& field, vector_t& vec);

typedef map<string, string> record_t;
void map2str(string& buf, const record_t& record, bool encode = true);
void str2map(record_t& record, const string& buf, bool decode = true);

bool va_function(const char* format, ...);
void str_format(string& dst, const char* format, ...);

void escape_string(const string& sql, string& dest);
void hex2str(string& dst, const string& src);
void str2hex(string& dst, const string& src);

#endif