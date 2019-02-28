#ifndef __3DES_H__
#define __3DES_H__

#include <string>
using namespace std;

void des3str(const char *key, const char *indata,  string &strResult);
void un_des3str(const char *key, const char *szBcdEnData, string &strResult);

#endif
