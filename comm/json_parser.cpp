#include "json_parser.h"
#include "printf_utils.h"
#include "utils.h"
#include "json/reader.h"
#include "json/writer.h"
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

int json_to_map(map<string, string>& record, string& data) {
    Json::Reader reader;
    Json::Value value;
    if (!reader.parse(data, value)) {
        PRINTF_ERROR("parse data failed : %s", data.c_str());
        return -1;
    }
    Json::Value::Members member = value.getMemberNames();
    for (Json::Value::Members::iterator it = member.begin(); it != member.end();
         it++) {
        // if (value[*it].isString())
        if (value[*it].type() == Json::stringValue) {
            // record.insert(pair<string, string>(*it, value[*it].asString()));
            record[*it] = value[*it].asString();
        }
        // else if (value[*it].isDouble())
        else if (value[*it].type() == Json::realValue) {
            // record.insert(pair<string, string>(*it,
            // to_string(value[*it].asDouble())));
            record[*it] = to_string(value[*it].asDouble());
        }
        // else if (value[*it].isBool())
        else if (value[*it].type() == Json::booleanValue) {
            // record.insert(pair<string, string>(*it,
            // to_string(value[*it].asBool())));
            record[*it] = to_string(value[*it].asBool());
        }
        // else if (value[*it].isInt())
        else if (value[*it].type() == Json::intValue) {
            // record.insert(pair<string, string>(*it,
            // to_string(value[*it].asInt())));
            record[*it] = to_string(value[*it].asInt());
        }
        // else if (value[*it].isUInt())
        else if (value[*it].type() == Json::uintValue) {
            // record.insert(pair<string, string>(*it,
            // to_string(value[*it].asUInt())));
            record[*it] = to_string(value[*it].asUInt());
        }
    }
    return 0;
}

void map_to_json(string& data, const map<string, string>& record) {
    Json::Value object;
    map<string, string>::const_iterator it;
    for (it = record.begin(); it != record.end(); it++) {
        object[it->first] = it->second;
    }
    Json::FastWriter writer;
    data = writer.write(object);
}

int rapid_json_to_map(map<string, string>& record, string& data) {
    rapidjson::Document content_json_doc;
    content_json_doc.Parse(data.c_str());
    if (content_json_doc.HasParseError() || !content_json_doc.IsObject()) {
        PRINTF_ERROR("json error [%s]", data.c_str());
        return -1;
    }
    for (rapidjson::Value::ConstMemberIterator it =
             content_json_doc.MemberBegin();
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

void rapid_map_to_json(string& data, const map<string, string>& record) {
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    map<string, string>::const_iterator it;
    for (it = record.begin(); it != record.end(); it++) {
        document.AddMember(rapidjson::StringRef(it->first.c_str()),
                           rapidjson::StringRef(it->second.c_str()), allocator);
    }
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    data = buffer.GetString();
}
