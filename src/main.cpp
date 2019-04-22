#include "printf.h"
#include "daemon.h"
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "tool.h"
#define STRING(X) #X
#define AND(X) X##X

int main(int argc, char *argv[])
{
	/*
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " conf_file" << endl;
		cerr << "Sample: " << argv[0] << " ../conf/test.conf.xml" << endl;
		return -1;
	}
	*/
	PRINTF_DEBUG("server start success");
	//init_daemon(NULL);
	const char *str = STRING(test);
	cout << str << endl;
	int num = AND(8);
	cout << num << endl;
	PRINTF_DEBUG("TEST:%d", 10);
	return 0;
}
