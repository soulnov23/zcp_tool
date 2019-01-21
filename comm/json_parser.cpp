#include "json_parser.h"
#include "printf.h"
#include "json/reader.h"
#include <fstream>
using namespace std;

int file_to_json(string &data, const char *file_path)
{
	ifstream file;
	file.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		file.open(file_path);
		getline(file, data, (char)EOF);
	}
	catch (ifstream::failure e)
	{
		PRINTF_ERROR("open file failed : %s", file_path);
		return -1;
	}
	return 0;
}

int json_to_map(map<string, string> &record, string &data)
{
	Json::Reader reader;
	Json::Value value;
	if (!reader.parse(data, value))
	{
		PRINTF_ERROR("parse data failed : %s", data.c_str());
		return -1;
	}
	Json::Value::Members member = value.getMemberNames();
	for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++)
	{
		record.insert(pair<string, string>(*it, value[*it].asString()));
	}
	return 0;
}
