#include "printf.h"
#include "utils.h"
#include "json_parser.h"
#include "http_handle.h"
#include "sha.h"
#include <time.h>
#include "md5.h"

int main(int argc, char *argv[])
{
	map<string, string> data;
	data["merchantId"] 		= "2026";
	data["serviceId"]   	= "2719";
	data["referenceCode"] 	= "20190523195756";
	data["item"] 			= "product";
	data["price"] 			= "10.00";
	data["successUrl"] 		= "http://dev.api.unipay.qq.com/cgi-bin/os_payguru_provide.fcg";
	data["failureUrl"] 		= "http://api.unipay.qq.com/cgi-bin/os_payguru_provide.fcg";
	string temp = data["merchantId"] + data["serviceId"] + data["referenceCode"] + data["item"] + data["price"] +
				data["successUrl"] + data["failureUrl"] + "NU1W1pvF";
	data["key"]				= md5(temp);
	string post_data;
	map_to_json(post_data, data);
	PRINTF_DEBUG("post_data:[%s]", post_data.c_str());

	string url = "https://cp.payguru.com/token";
	string head = "Content-Type: application/json";
	vector<string> vecHeadInfo;
	vecHeadInfo.push_back(head);
	string recv_data;
	int err_code;
	string err_msg;
	int ret = http_proc(url, 3, &vecHeadInfo, post_data, recv_data, err_code, err_msg);
	//int ret = http_proc(url, 3, NULL, post_data, recv_data, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
	}
	PRINTF_DEBUG("recv_data:[%s]", recv_data.c_str());
	
	return 0;
}
