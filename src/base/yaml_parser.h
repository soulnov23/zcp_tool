#pragma once

#include <string>

#include "src/base/printf_util.h"
#include "yaml-cpp/yaml.h"

int yaml_load_data(YAML::Node& root, const std::string& data) {
    root = YAML::Load(data);
    if (root.IsNull()) {
        PRINTF_ERROR("Load err: %s", data.c_str());
        return -1;
    }
    return 0;
}

int yaml_load_file(YAML::Node& root, const std::string& file_path) {
    try {
        root = YAML::LoadFile(file_path);
        return 0;
    } catch (YAML::BadFile&) {
        PRINTF_ERROR("BadFile(%s)", file_path.c_str());
        return -1;
    } catch (...) {
        PRINTF_ERROR("Exception");
        return -1;
    }
}

template <typename T>
bool yaml_get_value(YAML::Node& root, const std::string& key, T& value) {
    try {
        value = root[key].as<T>();
        return true;
    } catch (YAML::InvalidNode&) {
        PRINTF_ERROR("InvalidNode(%s)", key.c_str());
        return false;
    } catch (...) {
        PRINTF_ERROR("Exception");
        return false;
    }
}