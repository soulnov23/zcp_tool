#include "file_utils.h"
#include "json_parser.h"
#include "printf_utils.h"

int main(int argc, char* argv[]) {
    /*
    Document document;
    document.SetArray();
    Value::AllocatorType& allocator = document.GetAllocator();
    Value array(kArrayType);
    for (int i = 0; i < 3; i++) {
        string value = "200.00";
        string currency = "RUB";
        bool flag = true;
        Value amount_object(kObjectType);
        amount_object.AddMember("value", StringRef(value.c_str()), allocator);
        amount_object.AddMember("currency", StringRef(currency.c_str()),
                                allocator);
        amount_object.AddMember("flag", flag, allocator);
        amount_object.AddMember("test", "yes", allocator);
        array.PushBack(amount_object, allocator);
    }
    document.AddMember("amount", array, allocator);
    */

    string data;
    if (file_to_string(data, "/home/zcp_tool/example/test.json") != 0) {
        return -1;
    }
    PRINTF_DEBUG("data[%s]", data.c_str());
    Document doc;
    if (string_to_json(doc, data) != 0) {
        return -1;
    }
    Value* user_object;
    get_object(doc, "user", user_object);
    Value* id_object;
    get_object(*user_object, "id", id_object);
    Value::MemberIterator member = id_object->FindMember("value");
    member->value.SetString("123456", doc.GetAllocator());
    string result;
    json_to_string(result, doc);
    PRINTF_DEBUG("result[%s]", result.c_str());
    return 0;
}
