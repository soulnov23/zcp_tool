#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include <string>
#include <map>
using namespace std;

int json_to_map(map<string, string>& record, string& data);
void map_to_json(string& data, const map<string, string>& record);

#endif