#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include <map>
#include <string>
using namespace std;
#include "third_party/rapidjson/document.h"
#include "third_party/rapidjson/rapidjson.h"
using namespace rapidjson;

int json_to_map(map<string, string>& record, string& data);
void json_to_string(string& data, Document& doc);
void json_to_string(string& data, Value& value);
void map_to_json(string& data, const map<string, string>& record);
int string_to_json(Document& doc, const string& data);

// 引用指针的方式返回，是为了返回实际的object地址，可以对json进行真正的修改
int get_object(Value& rapid_value, const char* name, Value*& value);
int get_array(Value& rapid_value, const char* name, Value*& value);
int get_string(Value& rapid_value, const char* name, string& value);
int get_int64(Value& rapid_value, const char* name, int64_t& value);
int get_uint64(Value& rapid_value, const char* name, uint64_t& value);
int get_double(Value& rapid_value, const char* name, double& value);
int get_bool(Value& rapid_value, const char* name, bool& value);

// 拷贝string的方式添加，避免局部变量string被释放了，导致rapidjson异常
void add_string(Value& rapid_value, const char* name, string& value, Document& doc);

#endif