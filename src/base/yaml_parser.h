#pragma once

#include <string>

#include "yaml-cpp/yaml.h"

int yaml_load_data(const std::string& data, YAML::Node& root);

int yaml_load_file(const std::string& file_path, YAML::Node& root);

// 需要自己处理异常
template <typename T>
T yaml_get_value(YAML::Node& root, const std::string& key) {
    T value;
    value = root[key].as<T>();
    return value;
}