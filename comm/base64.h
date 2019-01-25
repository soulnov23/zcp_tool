#ifndef __BASE64_H__
#define __BASE64_H__

#include <string>
using namespace std;

int base64_encode(const string &str_in, string &str_out);
int base64_decode(const string &str_in, string &str_out);

#endif