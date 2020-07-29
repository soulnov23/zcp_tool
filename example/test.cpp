#include <map>
#include <string>
#include <vector>
#include "printf_utils.h"
using namespace std;
#include "utils.h"

int main(int argc, char* argv[]) {
    string kv = "a1=1%261&a2=2%252&a3=3&a4=4&a5";
    map<string, string> map_test;
    str2map(map_test, kv);
    map<string, string>::iterator it;
    for (it = map_test.begin(); it != map_test.end(); it++) {
        PRINTF_DEBUG("key[%s] value[%s]", it->first.c_str(), it->second.c_str());
    }

    vector<string> vec;
    str2vec(kv, '&', vec);
    for (size_t i = 0; i < vec.size(); i++) {
        PRINTF_DEBUG("key[%s]", vec[i].c_str());
    }
    return 0;
}