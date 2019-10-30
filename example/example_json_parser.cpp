#include "printf_utils.h"
#include "json_parser.h"
#include "file_utils.h"
#include <iostream>
using namespace std;

template <typename T1, typename T2, typename T3>
void PRINTF_MAP(map<T1, T2> &record, T3 &it) {
	it = record.begin();
	while (it != record.end()) {
		cout << it->first << ":" << it->second << endl;
		it++;
	}
}

int main(int argc, char* argv[]) {
	string data;
	if (file_to_string(data, "/home/zcp_tool/conf/server.json") != 0) {
		PRINTF_ERROR("file_to_json error");
		return -1;
	}
	map<string, string> record;
	if (json_to_map(record, data) != 0) {
		PRINTF_ERROR("json_to_map error");
		return -1;
	}
	map<string, string>::iterator it;
	PRINTF_MAP(record, it);

	string biz_data;
	map<string, string> biz_record;
	if (json_to_map(biz_record, record["biz_content"]) != 0) {
		PRINTF_ERROR("json_to_map error");
		return -1;
	}
	map<string, string>::iterator biz_it;
	PRINTF_MAP(biz_record, biz_it);
	return 0;
}
