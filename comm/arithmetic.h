#if !defined _CODE_ARITHMETIC_H_

#define _CODE_ARITHMETIC_H_



typedef unsigned char* pbyte;

typedef unsigned char byte;

class CArithmetic  

{

public:

	void UnDes3(const char *key,const char * indata,char * res);

	void GenMac(const byte *content, int len, const byte *key, byte *mac);

	void GenYBSMac(const byte *content, int len, const byte *key, byte *mac);

	void GenSonKey(const char * tmk,const char * sid,char * tak);

	void Des3(const char *key,const char * indata,char * res);

	int Bcd2ToAscii(const char *bcd,int len,int align,char *ascii);

	int AsciiToBcd2(const char *ascii,int len,char *bcd,int mode=0);

	CArithmetic();

	virtual ~CArithmetic();



};



#endif 



