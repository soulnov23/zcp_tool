#pragma once

#include <map>
#include <string>

#include "tinyxml2.h"

// tinyxml2::XMLDocument不支持拷贝构造和赋值运算符，不能放到return返回
int xml_load_file(const char* file_path, tinyxml2::XMLDocument& doc);
int xml_load_data(const std::string& data, tinyxml2::XMLDocument& doc);

std::map<std::string, tinyxml2::XMLElement*> xml_get_all_node(tinyxml2::XMLDocument& doc);
std::map<std::string, tinyxml2::XMLElement*> xml_get_all_node(tinyxml2::XMLElement* node);
tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLDocument& doc, const std::string& key);
tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLElement* node, const std::string& key);

std::map<std::string, std::string> xml_get_all_attri(tinyxml2::XMLElement* node);
std::string xml_get_attri(tinyxml2::XMLElement* node, const std::string& key);