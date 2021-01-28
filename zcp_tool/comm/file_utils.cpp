#include "zcp_tool/comm/file_utils.h"
#include <fstream>
#include <streambuf>
#include "zcp_tool/comm/printf_utils.h"

string file_to_string(const char* file_path) {
    string data = "";
    ifstream file(file_path);
    if (!file) {
        PRINTF_ERROR("open file failed : %s", file_path);
        return data;
    }
    istreambuf_iterator<char> begin(file);
    istreambuf_iterator<char> end;
    data = move(string(begin, end));
    return data;
}