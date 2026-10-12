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

// 配置项可选时用这个重载：key不存在或者值为空返回默认值，不抛异常。
// 类型对不上仍然抛，那是配置写错了，不该被默认值掩盖
template <typename T>
T yaml_get_value(YAML::Node& root, const std::string& key, const T& default_value) {
    YAML::Node node = root[key];
    if (!node || node.IsNull()) {
        return default_value;
    }
    return node.as<T>();
}