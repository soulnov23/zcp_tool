#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

#include <map>
#include <string>
using namespace std;
#include "third_party/tinyxml2/tinyxml2.h"

int get_conf(tinyxml2::XMLDocument& doc, const char* file_path);
int get_conf(tinyxml2::XMLDocument& doc, const char* data, size_t len);

void get_node(map<string, tinyxml2::XMLElement*>& record, tinyxml2::XMLDocument& doc);
void get_node(map<string, tinyxml2::XMLElement*>& record, tinyxml2::XMLElement* node);

void get_attri(map<string, string>& record, tinyxml2::XMLElement* node);

int xml_to_map(map<string, string>& record, const char* file_path);
int xml_to_map(map<string, string>& record, const char* data, size_t len);

#endif