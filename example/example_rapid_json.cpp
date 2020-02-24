#include "printf_utils.h"
#include <iostream>
using namespace std;
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filereadstream.h"
using namespace rapidjson;
#include <stdio.h>

static bool get_obj(Value& gvalue, const char* member, Value& node) {
  Value::MemberIterator mem = gvalue.FindMember(member);
  if ((mem == gvalue.MemberEnd()) || (!mem->value.IsObject())) {
    return false;
  }
  node = mem->value.GetObject();
  return true;
}

int main(int argc, char* argv[]) {
  /*
  Document document;
document.SetArray();
Value::AllocatorType& allocator = document.GetAllocator();
  //Value array(kArrayType);
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
  if (doc.HasParseError() || !doc.IsObject()) {
          PRINTF_ERROR();
  return -1;
  }
  Value::MemberIterator mem_amount = doc.FindMember("amount");
  if ((mem_amount == doc.MemberEnd()) || (!mem_amount->value.IsObject())) {
          PRINTF_ERROR();
  return -1;
  }
  Value amount_object_new(kObjectType);
  amount_object_new = mem_amount->value.GetObject();
  Value::MemberIterator mem_test = amount_object_new.FindMember("test");
  if ((mem_test == amount_object_new.MemberEnd()) ||
(!mem_test->value.IsString())) {
          PRINTF_ERROR();
  return -1;
  }
  string str_test = mem_test->value.GetString();
  PRINTF_DEBUG("test:%s", str_test.c_str());
  Value::MemberIterator flag_test = amount_object_new.FindMember("flag");
  if ((flag_test == amount_object_new.MemberEnd()) ||
(!flag_test->value.IsBool())) {
          PRINTF_ERROR();
  return -1;
  }
  bool flag = flag_test->value.GetBool();
  PRINTF_DEBUG("flag:%d", flag);

  FILE* fp = fopen("test.json", "r");
  char buff[65536];
  FileReadStream is(fp, buff, sizeof(buff));
  Document doc;
doc.ParseStream(is);
if (doc.HasParseError() || !doc.IsObject()) {
          PRINTF_ERROR("json error json=[%s]", buff);
  return -1;
  }
  Value::MemberIterator response_mem = doc.FindMember("response");
if ((response_mem == doc.MemberEnd()) || !response_mem->value.IsObject()) {
  PRINTF_ERROR("response is not in json or not int format");
          return -1;
}
  Value response_object(kObjectType);
  response_object = response_mem->value.GetObject();
  Value::MemberIterator result_mem = response_object.FindMember("result");
  if ((result_mem == response_object.MemberEnd()) ||
!result_mem->value.IsString()) {
  PRINTF_ERROR("result is not in json or not int format");
          return -1;
}
  string result = result_mem->value.GetString();
if (result != "SUCCESS") {
  Value::MemberIterator result_detail_mem =
response_object.FindMember("resultDetail");
      if ((result_detail_mem == response_object.MemberEnd()) ||
!result_detail_mem->value.IsString())
  {
      PRINTF_ERROR("resultDetail is not in json or not int format");
              return -1;
  }
  string result_detail = result_detail_mem->value.GetString();
  PRINTF_ERROR("%s", result_detail.c_str());
          return -1;
}
Value::MemberIterator token_data_mem = doc.FindMember("tokenData");
if ((token_data_mem == doc.MemberEnd()) || !token_data_mem->value.IsObject()) {
  PRINTF_ERROR("tokenData is not in json or not int format");
          return -1;
}
Value token_data_object(kObjectType);
token_data_object = token_data_mem->value.GetObject();
  Value::MemberIterator redirect_url_mem =
token_data_object.FindMember("redirectUrl");
  if ((redirect_url_mem == token_data_object.MemberEnd()) ||
!redirect_url_mem->value.IsString()) {
  PRINTF_ERROR("redirectUrl is not in json or not int format");
          return -1;
}
  string pay_url = redirect_url_mem->value.GetString();
  PRINTF_ERROR("pay_url:%s", pay_url.c_str());
  fclose(fp);
*/
  FILE* fp = fopen("test.json", "r");
  char buff[65536];
  FileReadStream is(fp, buff, sizeof(buff));
  Document doc;
  doc.ParseStream(is);
  if (doc.HasParseError() || !doc.IsObject()) {
    PRINTF_ERROR("json error json=[%s]", buff);
    return -1;
  }
  /*
  Value::MemberIterator resource_mem = doc.FindMember("resource");
if ((resource_mem == doc.MemberEnd()) || !resource_mem->value.IsObject()) {
  PRINTF_ERROR("resource is not in json or not int format");
          return -1;
}
  Value resource_object(kObjectType);
  resource_object = resource_mem->value.GetObject();
  */
  Value resource_object(kObjectType);
  get_obj(doc, "resource", resource_object);

  Value::MemberIterator purchase_units_mem =
      resource_object.FindMember("purchase_units");
  if ((purchase_units_mem == doc.MemberEnd()) ||
      !purchase_units_mem->value.IsArray()) {
    PRINTF_ERROR("purchase_units is not in json or not int format");
    return -1;
  }
  Value purchase_units_object(kArrayType);
  purchase_units_object = purchase_units_mem->value.GetArray();
  for (Value::ValueIterator it = purchase_units_object.Begin();
       it != purchase_units_object.End(); it++) {
    Value::MemberIterator invoice_id_mem = it->FindMember("invoice_id");
    if ((invoice_id_mem == it->MemberEnd()) ||
        !invoice_id_mem->value.IsString()) {
      PRINTF_ERROR("invoice_id is not in json or not int format");
      return -1;
    }
    string invoice_id = invoice_id_mem->value.GetString();
    PRINTF_DEBUG("invoice_id[%s]", invoice_id.c_str());
  }

  /*
  Value temp_object(kObjectType);
  temp_object = purchase_units_object[0].GetObject();
  */

  return 0;
}
