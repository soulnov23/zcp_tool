#include "json_parser.h"
#include "printf_utils.h"
#include "utils.h"
#include "writer.h"
#include "stringbuffer.h"

int json_to_map(map<string, string>& record, string& data) {
    Document content_json_doc;
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

void json_to_string(string& data, Document& doc) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    data = buffer.GetString();
}

void map_to_json(string& data, const map<string, string>& record) {
    Document document;
    document.SetObject();
    auto& allocator = document.GetAllocator();
    // 右值引用减少内存拷贝
    for (auto && [ first, second ] : record) {
        document.AddMember(StringRef(first.c_str()), StringRef(second.c_str()),
                           allocator);
    }
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    data = buffer.GetString();
}

int string_to_json(Document& doc, const string& data) {
    doc.Parse(data.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        PRINTF_ERROR("json error [%s]", data.c_str());
        return -1;
    }
    return 0;
}

int get_object(Value& rapid_value, const char* member_name, Value*& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsObject())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = &(member->value);
    return 0;
}

int get_array(Value& rapid_value, const char* member_name, Value*& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsArray())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = &(member->value);
    return 0;
}

int get_string(Value& rapid_value, const char* member_name, string& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsString())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = member->value.GetString();
    return 0;
}

int get_int64(Value& rapid_value, const char* member_name, string& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsInt64())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = to_string(member->value.GetInt64());
    return 0;
}

int get_uint64(Value& rapid_value, const char* member_name, string& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsUint64())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = to_string(member->value.GetUint64());
    return 0;
}

int get_double(Value& rapid_value, const char* member_name, string& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsDouble())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = to_string(member->value.GetDouble());
    return 0;
}

int get_bool(Value& rapid_value, const char* member_name, bool& value) {
    Value::MemberIterator member = rapid_value.FindMember(member_name);
    if ((member == rapid_value.MemberEnd()) || (!member->value.IsBool())) {
        string ret_info =
            string(member_name) + " is not in json or not int format";
        PRINTF_ERROR("%s", ret_info.c_str());
        return -1;
    }
    value = member->value.GetBool();
    return 0;
}