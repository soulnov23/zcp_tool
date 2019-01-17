#include "tinyxml2.h"
using namespace tinyxml2;
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	XMLDocument doc;
	XMLError ret = doc.LoadFile("/home/zcp_tools/conf/http_service.xml");
	if (ret != XML_SUCCESS)
	{
		cout << "doc.LoadFile failed" << endl;
		return doc.ErrorID();
	}
	XMLElement *mpay = doc.FirstChildElement("t_service_conf")->FirstChildElement("v3")->FirstChildElement("r")->FirstChildElement("mpay");
	XMLElement *node = mpay->FirstChildElement();
	while (node)
	{
		cout << node->Name() << endl;
		const XMLAttribute *attr = node->FirstAttribute();
		while (attr)
		{
			cout << attr->Name() << ":" << attr->Value() << endl;
			attr = attr->Next();
		}
		node = node->NextSiblingElement();
	}
	return 0;
}
