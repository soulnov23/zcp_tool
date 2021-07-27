#include <map>
#include <vector>

#include "yaml-cpp/yaml.h"

int main(int argc, char* argv[]) {
    YAML::Value root = YAML::Parse("test.yaml");
    return 0;
}