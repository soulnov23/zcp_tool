#include "printf.h"
#include "daemon.h"
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "tool.h"

int main(int argc, char *argv[])
{	
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " conf_file" << endl;
		cerr << "Sample: " << argv[0] << " ../conf/test.conf.xml" << endl;
		return -1;
	}
	PRINTF_DEBUG("server start success");
	//init_daemon(NULL);
	return 0;
}
