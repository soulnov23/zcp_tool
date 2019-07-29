#include "printf.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "http_handle.h"
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filereadstream.h"
using namespace rapidjson;
#include "time_utils.h"
#include "md5.h"
#include "json_parser.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    string temp = string("1") + string("10.56.44.120") + string("xyzedfbc");
    string key = md5(temp);
    /*
    for (string::iterator i = key.begin(); i != key.end(); i++)
    {
        *i = tolower(*i);
    }
    */
    map<string, string> map_data;
    map_data["merchant"] = "1";
    map_data["key"] = key;
    string post_string;
    map2str(post_string, map_data);
	PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
    string rsp_string;
	int err_code;
	string err_msg;
	string url = "https://sandbox.payguru.com/phs/getToken";
    vector<string> HeadInfo;
    //HeadInfo.push_back("Accept: */*");
    int ret = http_proc(url, 10, &HeadInfo, post_string, rsp_string, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
		return -1;
	}
	PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());

    Document doc;
    doc.Parse(rsp_string.c_str());
    if (doc.HasParseError() || !doc.IsObject())
    {
        PRINTF_ERROR("json error json=[%s]", rsp_string.c_str());
        return -1;
    }
    Value::MemberIterator status_mem = doc.FindMember("status");
    if ((status_mem == doc.MemberEnd()) || !status_mem->value.IsString())
    {
        PRINTF_ERROR("status is not in json or not int format");
        return -1;
    }
    string status = status_mem->value.GetString();
    if (status != "000")
    {
        Value::MemberIterator message_mem = doc.FindMember("message");
        if ((message_mem == doc.MemberEnd()) || !message_mem->value.IsString())
        {
            PRINTF_ERROR("message is not in json or not int format");
            return -1;
        }
        string message = message_mem->value.GetString();
        PRINTF_ERROR("%s", message.c_str());
        return -1;
    }
    Value::MemberIterator token_mem = doc.FindMember("token");
    if ((token_mem == doc.MemberEnd()) || !token_mem->value.IsString())
    {
        PRINTF_ERROR("token is not in json or not int format");
        return -1;
    }
    string token = token_mem->value.GetString();
	PRINTF_DEBUG("token:[%s]", token.c_str());

    time_t t = time(NULL);
    map<string, string> new_map_data;
    new_map_data["code"] = "demo";
    new_map_data["amount"] = "12.34";
    new_map_data["order"] = to_string(t);
    /*
    new_map_data["tc_no"] = "123654987";
    new_map_data["name"] = "chengpzhang";
    new_map_data["email"] = "chengpzhang@tencent.com";
    new_map_data["phone"] = "13026697461";
    new_map_data["user_ip"] = "9.134.3.96";
    new_map_data["apply_time"] = date2str_time(t);
    */
    new_map_data["description"] = "hello world!";
    string new_post_string;
    map2str(new_post_string, new_map_data);
	PRINTF_DEBUG("new_post_string:[%s]", new_post_string.c_str());
    string new_rsp_string;
	int new_err_code;
	string new_err_msg;
	string new_url = "https://sandbox.payguru.com/phs/orderToReference/direct";
    vector<string> vecHeadInfo;
    vecHeadInfo.push_back("merchant: 1");
    string token_head = string("token: ") + token;
    vecHeadInfo.push_back(token_head);
    int new_ret = http_proc(new_url, 10, &vecHeadInfo, new_post_string, new_rsp_string, new_err_code, new_err_msg);
	if (new_ret != 0)
	{
		PRINTF_ERROR("new_err_code:[%d] new_err_msg:[%s]", new_err_code, new_err_msg.c_str());
		return -1;
	}
	PRINTF_DEBUG("new_rsp_string:[%s]", new_rsp_string.c_str());

	return 0;
}