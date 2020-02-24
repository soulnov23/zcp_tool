#include "xml_parser.h"
#include "printf_utils.h"

int get_conf(tinyxml2::XMLDocument& doc, const char* file_path) {
    tinyxml2::XMLError ret = doc.LoadFile(file_path);
    if (ret != tinyxml2::XML_SUCCESS) {
        PRINTF_ERROR("load file failed : %s", file_path);
        return -1;
    }
    return 0;
}

int get_conf(tinyxml2::XMLDocument& doc, const char* data, size_t len) {
    tinyxml2::XMLError ret = doc.Parse(data, len);
    if (ret != tinyxml2::XML_SUCCESS) {
        PRINTF_ERROR("parse data failed : %s", data);
        return -1;
    }
    return 0;
}

void get_node(map<string, tinyxml2::XMLElement*>& record,
              tinyxml2::XMLDocument& doc) {
    tinyxml2::XMLElement* node = doc.FirstChildElement();
    while (node != NULL) {
        record.insert(pair<string, tinyxml2::XMLElement*>(node->Name(), node));
        node = node->NextSiblingElement();
    }
}

void get_node(map<string, tinyxml2::XMLElement*>& record,
              tinyxml2::XMLElement* node) {
    tinyxml2::XMLElement* child_node = node->FirstChildElement();
    while (child_node != NULL) {
        record.insert(pair<string, tinyxml2::XMLElement*>(child_node->Name(),
                                                          child_node));
        child_node = child_node->NextSiblingElement();
    }
}

void get_attri(map<string, string>& record, tinyxml2::XMLElement* node) {
    const tinyxml2::XMLAttribute* attri = node->FirstAttribute();
    while (attri != NULL) {
        record.insert(pair<string, string>(attri->Name(), attri->Value()));
        attri = attri->Next();
    }
}

int xml_to_map(map<string, string>& record, const char* file_path) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError ret = doc.LoadFile(file_path);
    if (ret != tinyxml2::XML_SUCCESS) {
        PRINTF_ERROR("load file failed : %s", file_path);
        return -1;
    }
    tinyxml2::XMLElement* root = doc.RootElement();
    tinyxml2::XMLElement* node = root->FirstChildElement();
    while (node != NULL) {
        const tinyxml2::XMLAttribute* attri = node->FirstAttribute();
        while (attri != NULL) {
            record.insert(pair<string, string>(attri->Name(), attri->Value()));
            attri = attri->Next();
        }
        node = node->NextSiblingElement();
    }
    return 0;
}

int xml_to_map(map<string, string>& record, const char* data, size_t len) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError ret = doc.Parse(data, len);
    if (ret != tinyxml2::XML_SUCCESS) {
        PRINTF_ERROR("parse data failed : %s", data);
        return -1;
    }
    tinyxml2::XMLElement* root = doc.RootElement();
    tinyxml2::XMLElement* node = root->FirstChildElement();
    while (node != NULL) {
        const tinyxml2::XMLAttribute* attri = node->FirstAttribute();
        while (attri != NULL) {
            record.insert(pair<string, string>(attri->Name(), attri->Value()));
            attri = attri->Next();
        }
        node = node->NextSiblingElement();
    }
    return 0;
}
