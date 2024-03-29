#include "src/base/yaml_parser.h"

#include "src/base/log.h"

int yaml_load_data(const std::string& data, YAML::Node& root) {
    int ret = 0;
    root = YAML::Load(data);
    if (root.IsNull()) {
        ret = -1;
        LOG_ERROR("Load data: {} error", data);
    }
    return ret;
}

int yaml_load_file(const std::string& file_path, YAML::Node& root) {
    int ret = 0;
    try {
        root = YAML::LoadFile(file_path);
    } catch (const std::exception& ex) {
        ret = -1;
        LOG_ERROR("{}", ex.what());
    } catch (...) {
        ret = 1;
        LOG_ERROR("unknow exception");
    }
    return ret;
}