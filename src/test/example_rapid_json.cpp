#include "src/base/file_utils.h"
#include "src/base/json_parser.h"
#include "src/base/printf_utils.h"

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

    string data = move(file_to_string("/data/home/project/src/test/test.json"));
    if (data.empty()) {
        return -1;
    }
    PRINTF_DEBUG("data[%s]", data.c_str());
    Document doc = move(string_to_json(data));
    if (doc.ObjectEmpty()) {
        return -1;
    }
    Value* user_object           = get_object(doc, "user");
    Value* id_object             = get_object(*user_object, "id");
    Value::MemberIterator member = id_object->FindMember("value");
    member->value.SetString("123456", doc.GetAllocator());
    string result = move(json_to_string(doc));
    PRINTF_DEBUG("result[%s]", result.c_str());
    return 0;
}
