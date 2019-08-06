#include "printf.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "http_handle.h"
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filereadstream.h"
using namespace rapidjson;
#include "time_utils.h"
#include "md5.h"
#include "json_parser.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    double amount = 48.99;
    char szbuf[64];
	snprintf(szbuf, sizeof(szbuf), "%.2lf", amount);
	PRINTF_DEBUG("[%s]", szbuf);

    char new_szbuf[64];
	snprintf(new_szbuf, sizeof(new_szbuf), "%.2lf", atof(szbuf)/10.00);
	PRINTF_DEBUG("[%s]", new_szbuf);


	return 0;
}