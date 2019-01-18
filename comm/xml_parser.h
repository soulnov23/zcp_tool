#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

#include <string>
#include <map>
using namespace std;
#include "tinyxml2.h"
using namespace tinyxml2;

int get_conf(XMLDocument &doc, const char *file_path);
int get_conf(XMLDocument &doc, const char *data, size_t len);

void get_node(map<string, XMLElement*> &record, XMLDocument &doc);
void get_node(map<string, XMLElement*> &record, XMLElement *node);

void get_attri(map<string, string> &record, XMLElement *node);

#endif