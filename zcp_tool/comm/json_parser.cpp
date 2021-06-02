#include "zcp_tool/comm/json_parser.h"

#include "third_party/rapidjson/stringbuffer.h"
#include "third_party/rapidjson/writer.h"
#include "zcp_tool/comm/printf_utils.h"
#include "zcp_tool/comm/string_utils.h"

map<string, string> json_to_map(const string& data) {
    map<string, string> record;
    Document content_json_doc;
    content_json_doc.Parse(data.c_str());
    if (content_json_doc.HasParseError() || !content_json_doc.IsObject()) {
        PRINTF_ERROR("json error [%s]", data.c_str());
        return record;
    }
    for (auto it = content_json_doc.MemberBegin(); it != content_json_doc.MemberEnd(); it++) {
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
    return record;
}

string json_to_string(Document& doc) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    string data = buffer.GetString();
    return data;
}

string json_to_string(Value& value) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    value.Accept(writer);
    string data = buffer.GetString();
    return data;
}

string map_to_json(const map<string, string>& record) {
    Document document;
    document.SetObject();
    auto& allocator = document.GetAllocator();
    for (const auto& [first, second] : record) {
        document.AddMember(StringRef(first.c_str()), StringRef(second.c_str()), allocator);
    }
    return json_to_string(document);
}

Document string_to_json(const string& data) {
    Document doc;
    doc.Parse(data.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        PRINTF_ERROR("json error [%s]", data.c_str());
    }
    return doc;
}

Value* get_object(Value& rapid_value, const char* name) {
    Value* value                 = nullptr;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsObject())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = &(member->value);
    return value;
}

Value* get_array(Value& rapid_value, const char* name) {
    Value* value                 = nullptr;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsArray())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = &(member->value);
    return 0;
}

string get_string(Value& rapid_value, const char* name) {
    string value;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsString())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = member->value.GetString();
    return value;
}

int64_t get_int64(Value& rapid_value, const char* name) {
    int64_t value                = 0;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsInt64())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = member->value.GetInt64();
    return value;
}

uint64_t get_uint64(Value& rapid_value, const char* name) {
    uint64_t value               = 0;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsUint64())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = member->value.GetUint64();
    return value;
}

double get_double(Value& rapid_value, const char* name) {
    double value                 = 0.000000;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsDouble())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = member->value.GetDouble();
    return value;
}

bool get_bool(Value& rapid_value, const char* name) {
    bool value                   = true;
    Value::MemberIterator member = rapid_value.FindMember(name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsBool())) {
        string ret_info = move(string(name) + " is not in json or not int format");
        PRINTF_ERROR("%s", ret_info.c_str());
        return value;
    }
    value = member->value.GetBool();
    return value;
}

void add_string(Value& rapid_value, const char* name, string& value, Document& doc) {
    Value k;
    k.SetString(name, doc.GetAllocator());
    Value v;
    k.SetString(value.c_str(), doc.GetAllocator());
    rapid_value.AddMember(k, v, doc.GetAllocator());
}