#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>
using namespace std;

class config
{
public:
	config();
	~config();

	int get_conf(const char *file_path);
	string get_value(string key);

public:
	map<string, string> record;
};

#endif