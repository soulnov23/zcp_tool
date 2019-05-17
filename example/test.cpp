#include "printf.h"
#include "utils.h"
#include "json_parser.h"
#include "http_handle.h"
#include "sha.h"
#include <time.h>

int main(int argc, char *argv[])
{
	map<string, string> data;
	data["appId"] 		= "24152";
	data["requestId"]   = "123456789";
	data["reqTime"]     = to_string(time(NULL));
	data["mdmId"] 		= "50283013";
	data["uin"] 		= "100000004572";
	data["bizType"] 	= "OVERSEA_CLOUD_SG";
	data["accountType"] = "0";
	string post_data;
	map_to_json(post_data, data);

	map<string, string> params;
	params["appid"] 	= "24152";
	params["timestamp"] = data["reqTime"];
	params["nonce"] 	= "ibuaiVcKdpRxkhJA";
	string temp_sign_param;
	map2str(temp_sign_param, params);
	string sign_param = temp_sign_param + "&key=7B00127AD637C55EE9988E7DD519A9B0395BC276";
	PRINTF_DEBUG("sign_param:[%s]", sign_param.c_str());
	string sign = hmac_sha256_hex("7B00127AD637C55EE9988E7DD519A9B0395BC276", sign_param);
	for (string::iterator i = sign.begin(); i != sign.end(); i++)
	{
		*i = toupper(*i);
	}
	params["signature"] = sign;
	string url_data;
	map2str(url_data, params);
	string temp_url = "http://srv.test-jarvis-api.oss.oa.com/api/Receipt/Va/1.0/applyVa?";
	string url = temp_url + url_data;
	PRINTF_DEBUG("url:[%s] data:[%s]", url.c_str(), post_data.c_str());
	string recv_data;
	int err_code;
	string err_msg;
	string head = "Content-Type: application/json";
	vector<string> vecHeadInfo;
	vecHeadInfo.push_back(head);
	int ret = http_proc(url, 3, &vecHeadInfo, post_data, recv_data, err_code, err_msg);
	//int ret = http_proc(url, 3, NULL, post_data, recv_data, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
	}
	PRINTF_DEBUG("recv_data:[%s]", recv_data.c_str());

/*
	map<string, string> data;
	data["appId"] 		= "24152";
	data["requestId"]   = to_string(time(NULL));
	data["reqTime"]     = to_string(time(NULL));
	data["pageSize"] 	= "1000";
	data["pageIndex"] 	= "1";
	data["bizType"] 	= "OVERSEA_CLOUD_SG";
	data["receiptDate"] = "2019-04-04";
	string post_data;
	map_to_json(post_data, data);

	map<string, string> params;
	params["appid"] 	= "24152";
	params["timestamp"] = data["reqTime"];
	params["nonce"] 	= "ibuaiVcKdpRxkhJA";
	string temp_sign_param;
	map2str(temp_sign_param, params);
	string sign_param = temp_sign_param + "&key=7B00127AD637C55EE9988E7DD519A9B0395BC276";
	PRINTF_DEBUG("sign_param:[%s]", sign_param.c_str());
	string sign = hmac_sha256_hex("7B00127AD637C55EE9988E7DD519A9B0395BC276", sign_param);
	for (string::iterator i = sign.begin(); i != sign.end(); i++)
	{
		*i = toupper(*i);
	}
	params["signature"] = sign;
	string url_data;
	map2str(url_data, params);
	string temp_url = "http://srv.test-jarvis-api.oss.oa.com/api/Receipt/Query/1.0/queryReceipt?";
	string url = temp_url + url_data;
	PRINTF_DEBUG("url:[%s] data:[%s]", url.c_str(), post_data.c_str());
	string recv_data;
	int err_code;
	string err_msg;
	string head = "Content-Type: application/json";
	vector<string> vecHeadInfo;
	vecHeadInfo.push_back(head);
	int ret = http_proc(url, 3, &vecHeadInfo, post_data, recv_data, err_code, err_msg);
	//int ret = http_proc(url, 3, NULL, post_data, recv_data, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
	}
	PRINTF_DEBUG("recv_data:[%s]", recv_data.c_str());
*/
	return 0;
}
