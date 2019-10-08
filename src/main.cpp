#include "printf.h"
#include "xml_parser.h"
#include "server.h"
#include <unistd.h>

map<string, string> config;

int main(int argc, char *argv[]) {
	PRINTF_DEBUG("%s %s", argv[0], argv[1]);
	if (argc < 2) {
		PRINTF_ERROR("Usage: %s conf_file", argv[0]);
		PRINTF_ERROR("Sample: %s ../conf/test.conf.xml", argv[0]);
		return -1;
	}

	const char *file_path = argv[1];
	if (xml_to_map(config, file_path) != 0) {
		PRINTF_ERROR("xml_to_map error");
		return -1;
	}
	PRINTF_DEBUG("ip:%s port:%s count:%s", config["ip"].c_str(), config["port"].c_str(), config["count"].c_str());

	//int count = strtol(config["count"].c_str(), nullptr, 10);
	if (init_signal() != 0) {
		PRINTF_ERROR("init_signal error");
		return -1;
	}
	for (int i = 0; i < 1; i++) {
		if (fork_child() != 0) {
			PRINTF_ERROR("fork_child error");
			return -1;
		}
	}
	while (true) {
		
	}
	return 0;
}
