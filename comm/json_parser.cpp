#include "json_parser.h"
#include "printf_utils.h"
#include "utils.h"
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

int json_to_map(map<string, string>& record, string& data) {
    rapidjson::Document content_json_doc;
    content_json_doc.Parse(data.c_str());
    if (content_json_doc.HasParseError() || !content_json_doc.IsObject()) {
        PRINTF_ERROR("json error [%s]", data.c_str());
        return -1;
    }
    for (auto it = content_json_doc.MemberBegin();
         it != content_json_doc.MemberEnd(); it++) {
        if (it->value.IsInt()) {
            record[it->name.GetString()] = to_string(it->value.GetInt());
        } else if (it->value.IsInt64()) {
            record[it->name.GetString()] = to_string(it->value.GetInt64());
        } else if (it->value.IsBool()) {
            record[it->name.GetString()] = to_string(it->value.GetBool());
        } else if (it->value.IsUint()) {
            record[it->name.GetString()] = to_string(it->value.GetUint());
        } else if (it->value.IsUint64()) {
            record[it->name.GetString()] = to_string(it->value.GetUint64());
        } else if (it->value.IsDouble()) {
            record[it->name.GetString()] = to_string(it->value.GetDouble());
        } else if (it->value.IsString()) {
            record[it->name.GetString()] = it->value.GetString();
        } else {
            continue;
        }
    }
    return 0;
}

void map_to_json(string& data, const map<string, string>& record) {
    rapidjson::Document document;
    document.SetObject();
    auto& allocator = document.GetAllocator();
    // 右值引用减少内存拷贝
    for (auto&& [first, second] : record) {
        document.AddMember(rapidjson::StringRef(first.c_str()),
                           rapidjson::StringRef(second.c_str()), allocator);
    }
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    data = buffer.GetString();
}
