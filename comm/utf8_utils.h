#ifndef __UTF8_UTILS_H__
#define __UTF8_UTILS_H__

#include <string>
using namespace std;

bool is_legal_utf8(const string &data);
//charset是源字符串的编码
string to_utf8(const char *charset, const string &data);
//charset是目标字符串的编码
string utf8_to(const char *charset, const string &data);

#endif