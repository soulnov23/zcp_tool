#include "printf.h"
#include <iostream>
using namespace std;
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
using namespace rapidjson;

int main(int argc, char *argv[])
{
	Document document;
    document.SetArray();
    Document::AllocatorType& allocator = document.GetAllocator();
	//Value array(kArrayType);
	for (int i = 0; i < 3; i++)
	{
		string value = "200.00";
		string currency = "RUB";
		bool flag = true;
		Value amount_object(kObjectType);
		amount_object.AddMember("value", StringRef(value.c_str()), allocator);
		amount_object.AddMember("currency", StringRef(currency.c_str()), allocator);
		amount_object.AddMember("flag", flag, allocator);
		amount_object.AddMember("test", "yes", allocator);
		document.PushBack(amount_object, allocator);		
	}
	//document.PushBack(array, allocator);
	//document.AddMember("amount", amount_object, allocator);
	StringBuffer buffer;
  	Writer<StringBuffer> writer(buffer);
  	document.Accept(writer);
  	string post_string = buffer.GetString();
	cout << post_string << endl;

	Document doc;
	doc.Parse(post_string.c_str());
	if (doc.HasParseError() || !doc.IsObject())
	{
		PRINTF_ERROR();
        return -1;
	}
	Value::MemberIterator mem_amount = doc.FindMember("amount");
	if ((mem_amount == doc.MemberEnd()) || (!mem_amount->value.IsObject()))
	{
		PRINTF_ERROR();
        return -1;
	}
	Value amount_object_new(kObjectType);
	amount_object_new = mem_amount->value.GetObject();
	Value::MemberIterator mem_test = amount_object_new.FindMember("test");
	if ((mem_test == amount_object_new.MemberEnd()) || (!mem_test->value.IsString()))
	{
		PRINTF_ERROR();
        return -1;
	}
	string str_test = mem_test->value.GetString();
	PRINTF_DEBUG("test:%s", str_test.c_str());
	Value::MemberIterator flag_test = amount_object_new.FindMember("flag");
	if ((flag_test == amount_object_new.MemberEnd()) || (!flag_test->value.IsBool()))
	{
		PRINTF_ERROR();
        return -1;
	}
	bool flag = flag_test->value.GetBool();
	PRINTF_DEBUG("flag:%d", flag);

	return 0;
}
