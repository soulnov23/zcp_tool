#include "config.h"
#include "printf.h"
#include "daemon.h"
#include <iostream>
using namespace std;

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
	
	config conf;
	if (conf.get_conf(argv[1]) != 0)
	{
		PRINTF_ERROR("get_conf error");
	}
	return 0;
}
