#include "config.h"
#include "printf.h"
#include "tinyxml2.h"
using namespace tinyxml2;
#include <iostream>

config::config()
{
	
}

config::~config()
{
	
}

/*
static get_node(XMLDocument &doc, map<string, string> &record)
{
	XMLElement *node = doc.FirstChildElement();
	while (node != NULL)
	{
		const XMLAttribute *attri = node->FirstAttribute();
		while (attri != NULL)
		{
			string key = attri->Name();
			string value = attri->Value();
			record.insert(pair<string, string>(key, value));
			attri = attri->Next();
		}
		node = node->NextSiblingElement();
	}
}
*/

int config::get_conf(const char *file_path)
{
	XMLDocument doc;
	XMLError ret = doc.LoadFile(file_path);
	if (ret != XML_SUCCESS)
	{
		PRINTF_ERROR("load file failed : %s", file_path);
		return -1;
	}
	
	map<string, string>::const_iterator it = record.begin();
	while (it != record.end())
	{
		cout << it->first << ":" << it->second << endl;
	}
	return 0;
}

string config::get_value(string key)
{
	string value = "";
	map<string, string>::const_iterator it = record.find(key);
	it != record.end() ? (value = it->second) : (value = "");
	return value;
}
