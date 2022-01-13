#pragma once

#include <string>

#include "src/base/log.h"
#include "yaml-cpp/yaml.h"

int yaml_load_data(YAML::Node& root, const std::string& data) {
    root = YAML::Load(data);
    if (root.IsNull()) {
        CONSOLE_ERROR("Load err: {}", data.c_str());
        return -1;
    }
    return 0;
}

int yaml_load_file(YAML::Node& root, const std::string& file_path) {
    try {
        root = YAML::LoadFile(file_path);
        return 0;
    } catch (YAML::BadFile&) {
        CONSOLE_ERROR("BadFile({})", file_path.c_str());
        return -1;
    } catch (...) {
        CONSOLE_ERROR("Exception");
        return -1;
    }
}

template <typename T>
bool yaml_get_value(YAML::Node& root, const std::string& key, T& value) {
    try {
        value = root[key].as<T>();
        return true;
    } catch (YAML::InvalidNode&) {
        CONSOLE_ERROR("InvalidNode({})", key.c_str());
        return false;
    } catch (...) {
        CONSOLE_ERROR("Exception");
        return false;
    }
}