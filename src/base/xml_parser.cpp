#include "src/base/xml_parser.h"

#include "src/base/log.h"

int xml_load_file(const char* file_path, tinyxml2::XMLDocument& doc) {
    int ret = 0;
    if (doc.LoadFile(file_path) != tinyxml2::XML_SUCCESS) {
        ret = -1;
        CONSOLE_ERROR("load file err: {}", file_path);
    }
    return ret;
}

int xml_load_data(const std::string& data, tinyxml2::XMLDocument& doc) {
    int ret = 0;
    if (doc.Parse(data.c_str(), data.length()) != tinyxml2::XML_SUCCESS) {
        ret = -1;
        CONSOLE_ERROR("load data err: {}", data.c_str());
    }
    return ret;
}

std::map<std::string, tinyxml2::XMLElement*> xml_get_all_node(tinyxml2::XMLDocument& doc) {
    std::map<std::string, tinyxml2::XMLElement*> temp_map;
    using value_type = std::map<std::string, tinyxml2::XMLElement*>::value_type;
    auto* node = doc.FirstChildElement();
    while (node != nullptr) {
        temp_map.insert(value_type(node->Name(), node));
        node = node->NextSiblingElement();
    }
    return temp_map;
}

std::map<std::string, tinyxml2::XMLElement*> xml_get_all_node(tinyxml2::XMLElement* node) {
    std::map<std::string, tinyxml2::XMLElement*> temp_map;
    using value_type = std::map<std::string, tinyxml2::XMLElement*>::value_type;
    auto* child_node = node->FirstChildElement();
    while (child_node != nullptr) {
        temp_map.insert(value_type(child_node->Name(), child_node));
        child_node = child_node->NextSiblingElement();
    }
    return temp_map;
}

tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLDocument& doc, const std::string& key) {
    tinyxml2::XMLElement* temp_ptr = nullptr;
    auto* node = doc.FirstChildElement();
    while (node != nullptr) {
        if (key == node->Name()) {
            temp_ptr = node;
            break;
        }
        node = node->NextSiblingElement();
    }
    return temp_ptr;
}

tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLElement* node, const std::string& key) {
    tinyxml2::XMLElement* temp_ptr = nullptr;
    auto* child_node = node->FirstChildElement();
    while (child_node != nullptr) {
        if (key == child_node->Name()) {
            temp_ptr = child_node;
            break;
        }
        child_node = child_node->NextSiblingElement();
    }
    return temp_ptr;
}

std::map<std::string, std::string> xml_get_all_attri(tinyxml2::XMLElement* node) {
    std::map<std::string, std::string> temp_map;
    using value_type = std::map<std::string, std::string>::value_type;
    auto* attri = node->FirstAttribute();
    while (attri != nullptr) {
        temp_map.insert(value_type(attri->Name(), attri->Value()));
        attri = attri->Next();
    }
    return temp_map;
}

std::string xml_get_attri(tinyxml2::XMLElement* node, const std::string& key) {
    std::string temp_string;
    auto* attri = node->FirstAttribute();
    while (attri != nullptr) {
        if (key == attri->Name()) {
            temp_string = attri->Value();
            break;
        }
        attri = attri->Next();
    }
    return temp_string;
}