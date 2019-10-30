#include "printf_utils.h"
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "utils.h"
#include "json_parser.h"
#include "crypto/rsa.h"
#include "crypto/md5.h"
#include "net/http_handle.h"
#include "coder.h"

void get_header(string sign, vector<string> &HeadInfo) {
	string now_time = to_string(time(NULL));
	string app_id = "8ab74856-8772-45c9-96db-54cb30ab9f74";
	string app_key = "5b96f20a-011f-4254-8be8-9a5ceb2f317f";
	string param = "appid=" + app_id + "&" + 
				   "secret=" + app_key + "&" + 
				   "sign=" + sign + "&" +
				   "timestamp=" + now_time;
	string api_sign = md5(param);
	string_lower(api_sign);
	HeadInfo.push_back("Content-Type: application/json");
	HeadInfo.push_back("appid: 8ab74856-8772-45c9-96db-54cb30ab9f74");
	string timestamp = "timestamp: " + now_time;
	string str_api_sign = "apisign: " + api_sign;
	HeadInfo.push_back(timestamp);
	HeadInfo.push_back(str_api_sign);
	PRINTF_DEBUG("timestamp:[%s] apisign:[%s]", now_time.c_str(), api_sign.c_str());
}

int main(int argc, char* argv[]) {
	/*
	string private_key = "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCc540quYC9xzCMZeFOe8UmE3W5LWrqFd/"
						 "2DDSHQASxq8vmOiwFRSG2hsVXtjfmNLQNhtpTR0SGDUjkCsx+"
"SJH0JDnOfQ2xXHasO65Rnv2wrHs64P6U0aUrMWjgapjkmLwzRV12AKNAX77MGIocpcB0KZhk+0AVc6oQCBybV65JTGu+"
"pAyFLMJRtIP5kH3VMuXmig6VeiZAsjEewD/emxgK3cXejMQvqlNYFnCLsZ7ovAhr+"
"bhz6SHkOws3p80O6zfQbKfLzdSVaZK8FnwNPznUxAK77bRZN0zF3V9mL9+zrarvFPD5VkcVHNLj0DRzLmr2c5TbiCigs4+I+"
"NMfhpoLAgMBAAECggEAbM8GzoImDXV87WAZhtu+NFF6ahhc9EiHL5H3O3PhzXRdyiK9NEpkvrdnUxRCX5pc4qSJ8waRNoUv7zSt60VYMf6NN+zw+"
"fYtNfONR30CYOq76nDtGzbnW7TADiDeNmjU2plX3uVCUPoUzmSWIpevht7xl9XE8xtq7AM0E2YSrzEADcxtqQslM0uVOf+ki1eu0/"
"OwCz13FzPlPtnDwt2Lw9xxCxWqTgpN4oD5m6EWTqbognUIJ0EFD0dHXjrYnHXc+/Za5e+CDXYApHuhR9bifa1e4HMN084oLY+rkSXUV3+"
"Te0APPCfbeEeqvubziDmKxxKaWUq1wPbYi4c06ZQdgQKBgQDhF7zDWgiJFTgrLGmExJRKiR/3QZN4sugYE1itdRDJmiPV4xhWPXSsND3WtqR5+0otb/"
"hbzRa3cyl/RXV/1ZmBbE46fX2DKnmLQ1gP74iOuqWpfxjh/qpk+3kEY9aP57le/O0QEEPsJmqCsGM7XnzfNsxGAFYaDHooRbcGtv++"
"AwKBgQCycuvRUQjV4dxTuRJuwFbmdq4odSBMu7yCS4i5I9I73d3TGZBWfiXQWFmuiPh+pf3HdvMbgyA243Uv/NGapSmNvARXm0/eEyfTxV7+"
"GVdwLf3sSe8DQMCR1eJA9VzuS+jhCrHkFgyW3V/3ki66W8YITENlgC+VebOatfFE8i/ZWQKBgQCZ2VmhxFX1LFW53J86qgoZb+QzYdTkOJQ+cGq6FDunL/"
"2yYYfu2g527TYfHbMJ1OH8cH22cVVHiiUg4l7PQzWqqlZF0CQLlOqCb0MvkS8rLxOv6DkfrqrUXrV2dK7gqSegbwuxYQyryg4eyWTp3UlIX/"
"H7Hpu7LjAIeq4Anu/p9QKBgFMtpiYHM6segGi2F5VwKhF6uGs7TTb3O0MwmiZSQCiPnlpLzC/E1TNsO0FTryC5lrVnCKKGWHm9RF595eXDnr7mKM/"
"9IRlOrH3VvhWLEmrDxVxiifpmMFzJ6ZCFzi91SrO7HHhIns2jmpv3k7hiFsi/Y5roSUXPWJyAull82jjhAoGAaKujjF4HL91UXZFetkkKiBIpIrH5+"
"XbiX9z7H9/Tv8NSy/zTvXp3hFl3dr9gO722i/96dTq4th23Gqtih4cA9x8Wd7RChR9yAK/ffSj1lW6RhBWj1j2JCPFCm1TJD5iO3bIeuHm2sAuafKKoWT/"
"VCUkKRwt9Wwh9yF20vMQ3kFw=";

	string url = "https://api.cmburl.cn:48065/polypay/v1.0/mchorders/onlinepay";
	map<string, string> post_data;
	post_data["version"] 	= "0.0.1";
	post_data["encoding"] 	= "UTF-8";
	post_data["signMethod"] = "01";
	map<string, string> biz_content;
	biz_content["body"] 			= "年夜饭10人套餐";
	biz_content["merId"] 			= "123123456456789";
	biz_content["notifyUrl"] 		= "http://dev.api.unipay.qq.com/cgi-bin/bank_cmb_provide.fcg";
	biz_content["orderId"] 			= "201901111116123456";
	biz_content["spbillCreateIp"] 	= "10.56.44.120";
	biz_content["subAppId"] 		= "wx049e5d41b3d11752";
	biz_content["tradeType"] 		= "APP";
	biz_content["txnAmt"] 			= "1";
	string biz_content_data;
	map_to_json(biz_content_data, biz_content);
	post_data["biz_content"] = biz_content_data;
	string params;
	map2str(params, post_data);
	string sign;
	calculate_rsa2_sign(params, private_key, SIGN_CODE_BASE64, sign);
	post_data["sign"] = sign;
	string post_data_str;
	map_to_json(post_data_str, post_data);
	PRINTF_DEBUG("post_data=[%s]", post_data_str.c_str());
	vector<string> HeadInfo;
	get_header(sign, HeadInfo);
	string recv_data;
	int err_code;
	string err_msg;
	if (0 != http_proc(url, 10, &HeadInfo, post_data_str, recv_data, err_code, err_msg)) {
		PRINTF_ERROR("http_proc failed");
	}
	PRINTF_DEBUG("recv_data=[%s]", recv_data.c_str());
	*/
	string test("aaa,bbb");
	string temp;
	url_encode(test, temp);
	PRINTF_DEBUG("test:%s", temp.c_str());

	test = "aaa%2cbbb";
	url_decode(test, temp);
	PRINTF_DEBUG("test:%s", temp.c_str());
	return 0;
}
