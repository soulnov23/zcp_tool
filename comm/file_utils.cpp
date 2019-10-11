#include "file_utils.h"
#include <fstream>
#include <streambuf>
#include "printf_utils.h"

int file_to_string(string &data, const char *file_path) {
	ifstream file(file_path);
	if (!file) {
		PRINTF_ERROR("open file failed : %s", file_path);
		return -1;
	}
	istreambuf_iterator<char> begin(file);
	istreambuf_iterator<char> end;
	data = string(begin, end);
	return 0;
}