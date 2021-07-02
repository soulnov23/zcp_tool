#include <map>
#include <vector>

#include "third_party/fmt/ranges.h"

int main(int argc, char* argv[]) {
    std::vector<std::string> vec;
    vec.push_back("a");
    vec.push_back("b");
    vec.push_back("c");
    fmt::print("{}\n", vec);
    std::map<std::string, std::string> record;
    record["a"] = "1";
    record["b"] = "2";
    record["c"] = "3";
    fmt::print("{}\n", record);
    return 0;
}