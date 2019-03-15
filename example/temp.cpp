#include "printf.h"
#include "daemon.h"
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "tool.h"
#include "json_parser.h"
#include "rsa.h"
//#include "3des.h"

int main(int argc, char *argv[])
{	
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " conf_file" << endl;
		cerr << "Sample: " << argv[0] << " ../conf/test.conf.xml" << endl;
		return -1;
	}
	PRINTF_DEBUG("server start success");
	//init_daemon(NULL);
/*
	map<string, string> params;
	params["tranamt"] = "99";
	params["account"] = "150426";
	params["sno"] = "2015170269";
	params["toaccount"] = "1000000";
	params["thirdsystem"] = "cheshi";
	params["thirdorderid"] = "20190129123456";
	params["ordertype"] = "phone";
	params["orderdesc"] = "iphone7";
	params["thirdurl"] = "http://dev.api.unipay.qq.com/cgi-bin/g_bank_cmb_provide.fcg";
	string sign_param;
	map2str(sign_param, params);
	string new_sign_param;
	des3str("MGY0MzVmYTRhYmRhNDUyODhhNjAwODMy", sign_param.c_str(), new_sign_param);
	string sign;
	string private_key = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAL7UXJxVspeISrue"
"E10yoFHXLyWjC7LFjBFzJiDMvkDoxR1KnSiTnkFBchHjuW3K6bVt0KEiVYTYy9b1"
"WkMQc0fnPvPiW2yO5L4Vt6RCsBMof4Hv647WMd4RwFQxasYhp45Qzsp4FySU73Rq"
"P1ea9ZLMCrQvvrKA7fx5CBBKntzNAgMBAAECgYBUbJs0Ru2mAJQfgQXMG5O14V9o"
"ZUPNqXwYVDswE6SwDsr19Iam5vgCorOWmor0ueq/a3/7abcv+MHlrdJrDSISj1fB"
"VWwc5dUBsEwvqAquDJZFH8WJvbU4SFYaeHAUd/XjOhq8n3t4rh/JWcGo+wp4fy9r"
"s+0R9sRSEuB0O76VwQJBAO9veMFA/mincjugnDQK8kz9f36+cAA2Ud29XDjQE6Rz"
"5CeAe3uWEc7bvWlorgYKkD02BcuEDJT4K5bHpQAOApUCQQDMCA3LCfyxIH+9XDOV"
"jjTWB+I24O1Kt3SP2gvHj5TyDBAopyBCWBFva8i4F8gbjcIAE9tNvhn1lO6lYINX"
"QVtZAkEA2Q7hFbDp8c36zPtpH/Tce9dDgY760fZseOx1cjuP0wIMz5K2Xu9I7ZJf"
"P/PZ7zgcUqFq13i6P0JLocETX91TkQJBAIgw8BQRzIznZp0dyIkR7Y7YoFWFep+f"
"Kb1EDQzeeMvA5aBdPplKdAjt54OKzFYoH9Exb/fK50UYbtwROlqqE7ECQC4vkwAE"
"4sHrlAkfOpAU9w3tjAB3RK21dAmOJBtgM6YFvG1A64TgybrQZ3kf4268keFG2YKw"
"pCj8Z+3aXW/Rvh4=";
	calculate_rsa_sign(new_sign_param, private_key, SIGN_CODE_BASE64, sign);
	params["sign"] = sign;
	string data;
	map2str(data, params);
	PRINTF_DEBUG("data:[%s]", data.c_str());
*/
	return 0;
}
