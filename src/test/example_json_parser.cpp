#include <iostream>

#include "src/base/file_utils.h"
#include "src/base/json_parser.h"
#include "src/base/printf_utils.h"
using namespace std;

template <typename T1, typename T2, typename T3>
void PRINTF_MAP(map<T1, T2>& record, T3& it) {
    it = record.begin();
    while (it != record.end()) {
        cout << it->first << ":" << it->second << endl;
        it++;
    }
}

int main(int argc, char* argv[]) {
    string data = file_to_string("/home/src/conf/server.json");
    if (data.empty()) {
        PRINTF_ERROR("file_to_json error");
        return -1;
    }
    map<string, string> record = move(json_to_map(data));
    map<string, string>::iterator it;
    PRINTF_MAP(record, it);

    string biz_data;
    map<string, string> biz_record = move(json_to_map(record["biz_content"]));
    map<string, string>::iterator biz_it;
    PRINTF_MAP(biz_record, biz_it);
    return 0;
}
