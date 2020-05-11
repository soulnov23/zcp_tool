#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include <string>
#include <map>
using namespace std;
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
using namespace rapidjson;

int json_to_map(map<string, string>& record, string& data);
void map_to_json(string& data, const map<string, string>& record);

int get_document(Document& doc, const string& data);
int get_object(Value& rapid_value, const char* member_name, Value& value);
int get_array(Value& rapid_value, const char* member_name, Value& value);
int get_string(Value& rapid_value, const char* member_name, string& value);
int get_int64(Value& rapid_value, const char* member_name, string& value);
int get_uint64(Value& rapid_value, const char* member_name, string& value);
int get_double(Value& rapid_value, const char* member_name, string& value);
int get_bool(Value& rapid_value, const char* member_name, bool& value);

#endif