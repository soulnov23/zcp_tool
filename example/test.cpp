#include "printf.h"
#include "json_parser.h"
#include "stl_tool.h"

int main(int argc, char *argv[])
{
	string data;
	if (file_to_json(data, "/home/zcp_tool/conf/server.json") != 0)
	{
		PRINTF_ERROR("file_to_json error");
		return -1;
	}
	map<string, string> record;
	if (json_to_map(record, data) != 0)
	{
		PRINTF_ERROR("json_to_map error");
		return -1;
	}
	map<string, string>::iterator it;
	PRINTF_MAP(record, it);

	string biz_data;
	map<string, string> biz_record;
	if (json_to_map(biz_record, record["biz_content"]) != 0)
	{
		PRINTF_ERROR("json_to_map error");
		return -1;
	}
	map<string, string>::iterator biz_it;
	PRINTF_MAP(biz_record, biz_it);
	return 0;
}
