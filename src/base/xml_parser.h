#pragma once

#include <map>
#include <string>

#include "tinyxml2.h"

int xml_load_file(const char* file_path, tinyxml2::XMLDocument& doc);
int xml_load_data(std::string& data, tinyxml2::XMLDocument& doc);

void xml_get_all_node(tinyxml2::XMLDocument& doc, std::map<std::string, tinyxml2::XMLElement*>& record);
void xml_get_all_node(tinyxml2::XMLElement* node, std::map<std::string, tinyxml2::XMLElement*>& record);
tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLDocument& doc, const std::string& key);
tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLElement* node, const std::string& key);

void xml_get_all_attri(tinyxml2::XMLElement* node, std::map<std::string, std::string>& record);
std::string xml_get_attri(tinyxml2::XMLElement* node, const std::string& key);