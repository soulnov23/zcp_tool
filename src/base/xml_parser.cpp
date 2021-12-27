#include "src/base/xml_parser.h"

#include "src/base/printf_utils.h"

int xml_load_file(const char* file_path, tinyxml2::XMLDocument& doc) {
    auto ret = doc.LoadFile(file_path);
    if (ret != tinyxml2::XML_SUCCESS) {
        PRINTF_ERROR("load file failed : %s", file_path);
        return -1;
    }
    return 0;
}

int xml_load_data(std::string& data, tinyxml2::XMLDocument& doc) {
    auto ret = doc.Parse(data.c_str(), data.length());
    if (ret != tinyxml2::XML_SUCCESS) {
        PRINTF_ERROR("parse data failed : %s", data.c_str());
        return -1;
    }
    return 0;
}

void xml_get_all_node(tinyxml2::XMLDocument& doc, std::map<std::string, tinyxml2::XMLElement*>& record) {
    auto* node = doc.FirstChildElement();
    while (node != nullptr) {
        record.insert(pair<string, decltype(node)>(node->Name(), node));
        node = node->NextSiblingElement();
    }
}

void xml_get_all_node(tinyxml2::XMLElement* node, std::map<std::string, tinyxml2::XMLElement*>& record) {
    auto* child_node = node->FirstChildElement();
    while (child_node != nullptr) {
        record.insert(pair<string, decltype(child_node)>(child_node->Name(), child_node));
        child_node = child_node->NextSiblingElement();
    }
}

tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLDocument& doc, const std::string& key) {
    auto* node = doc.FirstChildElement();
    while (node != nullptr) {
        if (key == node->Name()) {
            return node;
        }
        node = node->NextSiblingElement();
    }
    return nullptr;
}

tinyxml2::XMLElement* xml_get_node(tinyxml2::XMLElement* node, const std::string& key) {
    auto* child_node = node->FirstChildElement();
    while (child_node != nullptr) {
        if (key == child_node->Name()) {
            return child_node;
        }
        child_node = child_node->NextSiblingElement();
    }
    return nullptr;
}

void xml_get_all_attri(tinyxml2::XMLElement* node, std::map<std::string, std::string>& record) {
    auto* attri = node->FirstAttribute();
    while (attri != nullptr) {
        record.insert(pair<string, string>(attri->Name(), attri->Value()));
        attri = attri->Next();
    }
}

std::string xml_get_attri(tinyxml2::XMLElement* node, const std::string& key) {
    auto* attri = node->FirstAttribute();
    while (attri != nullptr) {
        if (key == attri->Name()) {
            return attri->Value();
        }
        attri = attri->Next();
    }
    return "";
}