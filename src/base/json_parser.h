#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include <map>
#include <string>
using namespace std;
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
using namespace rapidjson;

map<string, string> json_to_map(const string& data);
string json_to_string(Document& doc);
string json_to_string(Value& value);
string map_to_json(const map<string, string>& record);
Document string_to_json(const string& data);

// 引用指针的方式返回，是为了返回实际的object地址，可以对json进行真正的修改
Value* get_object(Value& rapid_value, const char* name);
Value* get_array(Value& rapid_value, const char* name);
string get_string(Value& rapid_value, const char* name);
int64_t get_int64(Value& rapid_value, const char* name);
uint64_t get_uint64(Value& rapid_value, const char* name);
double get_double(Value& rapid_value, const char* name);
bool get_bool(Value& rapid_value, const char* name);

// 拷贝string的方式添加，避免局部变量string被释放了，导致rapidjson异常
void add_string(Value& rapid_value, const char* name, string& value, Document& doc);

#endif