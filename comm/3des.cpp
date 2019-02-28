#include "3des.h"
#include "arithmetic.h"
#include <string.h>

void des3str(const char *key, const char *indata,  string &strResult)
{
	CArithmetic ar;
	char szKey[16] = {0};

	char *srcStr;
	char *stEnData;
	char *szBcdEnData;

	int len=strlen(indata);
	int enLen= (len%8)==0?len:((len/8+1)*8);


	srcStr = new char[enLen+1];
	memset(srcStr, 0, enLen+1);
	strncpy(srcStr,indata, enLen );

	stEnData = new char[enLen+1];
	memset(stEnData, 0, enLen+1);

	int bcdlen= enLen*2;
	szBcdEnData=new char[bcdlen+1];
	memset(szBcdEnData, 0, bcdlen+1);

	ar.Bcd2ToAscii(key, 32, 0, szKey);


	int times= len/8 + (((len%8)==0)?0:1);

	for(int index=0; index<times ; index++)
	{
		ar.Des3(szKey, srcStr+8*index,  stEnData+8*index);
	}


	ar.AsciiToBcd2( stEnData, enLen, szBcdEnData);

	strResult=szBcdEnData ;

	delete [] srcStr;
	delete [] stEnData;
	delete [] szBcdEnData;

	return;
}

void un_des3str(const char *key, const char *szBcdEnData, string &strResult)
{
	CArithmetic ar;
	
	char szKey[16] = {0};
	char *stEnData, *szResult;

	int bcdLen = strlen(szBcdEnData);
	int enLen = ((bcdLen / 2) % 8) == 0 ? (bcdLen / 2) : (((bcdLen / 2) / 8 + 1)*8);
	
	stEnData = new char[enLen];
	memset(stEnData, 0, enLen);
	
	szResult = new char[enLen + 1];
	memset(szResult, 0, enLen + 1);

	
	ar.Bcd2ToAscii(key, 32, 0, szKey);
	ar.Bcd2ToAscii(szBcdEnData, bcdLen, 0, stEnData);
	
	unsigned int times = enLen / 8;
	for(unsigned int index = 0; index < times; index++)
	{
		ar.UnDes3(szKey, stEnData + 8 * index, szResult + 8 * index);
	}
	strResult = szResult;
	
	delete [] stEnData;
	delete [] szResult;

	return;
}

