#include "printf.h"
#include "daemon.h"
#include <iostream>
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

	string time_now;
	if (get_time_now(time_now) == 0)
	{
		PRINTF_ERROR("time_now:%s", time_now.c_str());
	}

	return 0;
}
