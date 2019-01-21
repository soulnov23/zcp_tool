#include "json_parser.h"
#include "printf.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

int get_conf(XMLDocument &doc, const char *file_path)
{
	XMLError ret = doc.LoadFile(file_path);
	if (ret != XML_SUCCESS)
	{
		PRINTF_ERROR("load file failed : %s", file_path);
		return -1;
	}
	return 0;
}

int get_conf(XMLDocument &doc, const char *data, size_t len)
{
	XMLError ret = doc.Parse(data, len);
	if (ret != XML_SUCCESS)
	{
		PRINTF_ERROR("parse data failed : %s", data);
		return -1;
	}
	return 0;
}

void get_node(map<string, XMLElement*> &record, XMLDocument &doc)
{
	XMLElement *node = doc.FirstChildElement();
	while (node != NULL)
	{
		record.insert(pair<string, XMLElement*>(node->Name(), node));
		node = node->NextSiblingElement();
	}
}

void get_node(map<string, XMLElement*> &record, XMLElement *node)
{
	XMLElement *child_node = node->FirstChildElement();
	while (child_node != NULL)
	{
		record.insert(pair<string, XMLElement*>(child_node->Name(), child_node));
		child_node = child_node->NextSiblingElement();
	}
}

void get_attri(map<string, string> &record, XMLElement *node)
{
	const XMLAttribute *attri = node->FirstAttribute();
	while (attri != NULL)
	{
		record.insert(pair<string, string>(attri->Name(), attri->Value()));
		attri = attri->Next();
	}	
}
