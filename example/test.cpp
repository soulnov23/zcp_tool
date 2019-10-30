#include "printf_utils.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
using namespace std;
#include "net/http_handle.h"
#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filereadstream.h"
using namespace rapidjson;
#include "time_utils.h"
#include "utils.h"
#include "json_parser.h"

int main(int argc, char* argv[])
{
	string card_no = "4567801230000010";
	string card_pin = "170033";
	string card_amount = "500";
	string amount = "60";
	vector<string> vecHeadInfo;
    vecHeadInfo.push_back("TerminalID: PUBG-MidasBuy-POS-01");
    vecHeadInfo.push_back("UserName: manager");
    vecHeadInfo.push_back("Password: welcome");
    vecHeadInfo.push_back("ForwardEntityId: proxima.com");
    vecHeadInfo.push_back("ForwardEntityPassword: com.proxima");
    vecHeadInfo.push_back("MerchantOutletName: PUBG-MidasBuy-01");
    vecHeadInfo.push_back("AcquirerId: Proxima BPL");
    vecHeadInfo.push_back("OrganizationName: Proxima BPL");
    vecHeadInfo.push_back("POSEntryMode: 2");
    vecHeadInfo.push_back("POSTypeId: 1");
    vecHeadInfo.push_back("POSName: PUBG-MidasBuy-POS-01");
	string date = date2str_time(get_time_sec());
    string date_at_client = string("DateAtClient: ") + date.substr(0, 10);
    vecHeadInfo.push_back(date_at_client);

    string rsp_string;
	int err_code;
	string err_msg;
	string url = "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/initialize";
	vector<string> vecHeadInfo1 = vecHeadInfo;
    string transaction_id = string("TransactionId: ") + to_string(get_time_sec());
    vecHeadInfo1.push_back(transaction_id);
    string post_string("");
    int ret = http_proc(url, 10, &vecHeadInfo1, post_string, rsp_string, err_code, err_msg);
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
    Value::MemberIterator api_web_mem = doc.FindMember("ApiWebProperties");
    if ((api_web_mem == doc.MemberEnd()) || !api_web_mem->value.IsObject())
    {
        PRINTF_ERROR("ApiWebProperties is not in json or not int format");
		return -1;
    }
	Value api_web_object(kObjectType);
	api_web_object = api_web_mem->value;

	Value::MemberIterator batch_no_mem = api_web_object.FindMember("CurrentBatchNumber");
	if ((batch_no_mem == api_web_object.MemberEnd()) || !batch_no_mem->value.IsInt())
    {
        PRINTF_ERROR("value is not in json or not int format");
		return -1;
    }
    string batch_no = to_string(batch_no_mem->value.GetInt());
    PRINTF_DEBUG("CurrentBatchNumber:[%s]", batch_no.c_str());

	string order_id = to_string(get_time_sec());
    map<string, string> map_data;
    map_data["Amount"] = amount;
	map_data["BillAmount"] = card_amount;
	map_data["InvoiceNumber"] = order_id;
	map_data["IdempotencyKey"] = "abcdefghi";
	map_data["CardNumber"] = card_no;
    map_data["CardPIN"] = card_pin;
    map_data["Notes"] = "desc";
	post_string.clear();
    rapid_map_to_json(post_string, map_data);
	PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
	url = "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/redeem";
	vector<string> vecHeadInfo2 = vecHeadInfo;
	transaction_id = string("TransactionId: ") + order_id;
    vecHeadInfo2.push_back(transaction_id);
	string current_batch_no = string("CurrentBatchNumber: ") + batch_no;
    vecHeadInfo2.push_back(current_batch_no);
	vecHeadInfo2.push_back("Content-Type: application/json");
	rsp_string.clear();
	/*
    ret = http_proc(url, 10, &vecHeadInfo2, post_string, rsp_string, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
		return -1;
	}
	PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
	*/
	//map<string, string> map_return;
	//rapid_json_to_map(map_return, rsp_string);
	//string approval_code = map_return["ApprovalCode"];

	map<string, string> new_map_data;
	new_map_data["CardNumber"] = card_no;
	new_map_data["OriginalInvoiceNumber"] = order_id;
    new_map_data["OriginalAmount"] = amount;
	new_map_data["OriginalBatchNumber"] = batch_no;
	new_map_data["OriginalTransactionId"] = order_id;
	//new_map_data["OriginalApprovalCode"] = approval_code;
	post_string.clear();
    rapid_map_to_json(post_string, new_map_data);
	PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
	url = "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/cancelredeem";
	vector<string> vecHeadInfo3 = vecHeadInfo;
    transaction_id = string("TransactionId: ") + to_string(get_time_sec());
    vecHeadInfo3.push_back(transaction_id);
	vecHeadInfo3.push_back(current_batch_no);
	vecHeadInfo3.push_back("Content-Type: application/json");
	rsp_string.clear();
    ret = http_proc(url, 10, &vecHeadInfo3, post_string, rsp_string, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
		return -1;
	}
	PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
/*
	map<string, string> new_new_map_data;
	new_new_map_data["CardNumber"] = card_no;
	post_string.clear();
    rapid_map_to_json(post_string, new_new_map_data);
	PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
	url = "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/balanceenquiry";
	vector<string> vecHeadInfo4 = vecHeadInfo;
    transaction_id = string("TransactionId: ") + to_string(get_time_sec());
    vecHeadInfo4.push_back(transaction_id);
	vecHeadInfo4.push_back(current_batch_no);
	vecHeadInfo4.push_back("Content-Type: application/json");
	rsp_string.clear();
    ret = http_proc(url, 10, &vecHeadInfo4, post_string, rsp_string, err_code, err_msg);
	if (ret != 0)
	{
		PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
		return -1;
	}
	PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
*/

	return 0;
}