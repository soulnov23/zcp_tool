#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "zcp_tool/comm/printf_utils.h"
using namespace std;
#include "third_party/rapidjson/document.h"
#include "third_party/rapidjson/filereadstream.h"
#include "third_party/rapidjson/rapidjson.h"
#include "third_party/rapidjson/stringbuffer.h"
#include "third_party/rapidjson/writer.h"
#include "zcp_tool/comm/net/http_handle.h"
using namespace rapidjson;
#include "zcp_tool/comm/json_parser.h"
#include "zcp_tool/comm/time_utils.h"
#include "zcp_tool/comm/utils.h"

string g_card_no     = "4567801230000220";
string g_card_pin    = "104143";
string g_card_amount = "1000";
string g_amount      = "10";
vector<string> g_vecHeadInfo;
string g_OriginalInvoiceNumber;
string g_OriginalBatchNumber;
string g_OriginalTransactionId;
string g_batch_no;

void init() {
    g_vecHeadInfo.push_back("TerminalID: PUBG-MidasBuy-POS-01");
    g_vecHeadInfo.push_back("UserName: manager");
    g_vecHeadInfo.push_back("Password: welcome");
    g_vecHeadInfo.push_back("ForwardEntityId: proxima.com");
    g_vecHeadInfo.push_back("ForwardEntityPassword: com.proxima");
    g_vecHeadInfo.push_back("MerchantOutletName: PUBG-MidasBuy-01");
    g_vecHeadInfo.push_back("AcquirerId: Proxima BPL");
    g_vecHeadInfo.push_back("OrganizationName: Proxima BPL");
    g_vecHeadInfo.push_back("POSEntryMode: 2");
    g_vecHeadInfo.push_back("POSTypeId: 1");
    g_vecHeadInfo.push_back("POSName: PUBG-MidasBuy-POS-01");
    string date           = date2str_time(get_time_sec());
    string date_at_client = string("DateAtClient: ") + date.substr(0, 10);
    g_vecHeadInfo.push_back(date_at_client);
}

int get_batch_no(string& batch_no) {
    string rsp_string;
    int err_code;
    string err_msg;
    string url                 = "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/initialize";
    vector<string> vecHeadInfo = g_vecHeadInfo;
    string post_string("");
    int ret = http_proc(url, 10, &vecHeadInfo, post_string, rsp_string, err_code, err_msg);
    if (ret != 0) {
        PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
        return -1;
    }
    // PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
    map<string, string> map_return = move(json_to_map(rsp_string));
    string rsp_code                = map_return["ResponseCode"];
    string rsp_msg                 = map_return["ResponseMessage"];
    if (rsp_code != "0") {
        PRINTF_ERROR("rsp_code[%s] rsp_msg[%s]", rsp_code.c_str(), rsp_msg.c_str());
        return -1;
    }

    Document doc;
    doc.Parse(rsp_string.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        PRINTF_ERROR("json error json=[%s]", rsp_string.c_str());
        return -1;
    }
    Value::MemberIterator api_web_mem = doc.FindMember("ApiWebProperties");
    if ((api_web_mem == doc.MemberEnd()) || !api_web_mem->value.IsObject()) {
        PRINTF_ERROR("ApiWebProperties is not in json or not int format");
        return -1;
    }
    Value api_web_object(kObjectType);
    api_web_object = api_web_mem->value;

    Value::MemberIterator batch_no_mem = api_web_object.FindMember("CurrentBatchNumber");
    if ((batch_no_mem == api_web_object.MemberEnd()) || !batch_no_mem->value.IsInt()) {
        PRINTF_ERROR("value is not in json or not int format");
        return -1;
    }
    g_batch_no = to_string(batch_no_mem->value.GetInt());
    PRINTF_DEBUG("CurrentBatchNumber[%s]", g_batch_no.c_str());
    return 0;
}

int balance_enquiry() {
    map<string, string> map_data;
    map_data["CardNumber"] = g_card_no;
    string post_string     = move(map_to_json(map_data));
    // PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
    string url =
        "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/"
        "balanceenquiry";
    vector<string> vecHeadInfo = g_vecHeadInfo;
    string current_batch_no    = string("CurrentBatchNumber: ") + g_batch_no;
    vecHeadInfo.push_back(current_batch_no);
    vecHeadInfo.push_back("Content-Type: application/json");
    int err_code;
    string err_msg;
    string rsp_string;
    int ret = http_proc(url, 10, &vecHeadInfo, post_string, rsp_string, err_code, err_msg);
    if (ret != 0) {
        PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
        return -1;
    }
    // PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
    map<string, string> map_return = move(json_to_map(rsp_string));
    string rsp_code                = map_return["ResponseCode"];
    string rsp_msg                 = map_return["ResponseMessage"];
    if (rsp_code != "0") {
        PRINTF_ERROR("rsp_code[%s] rsp_msg[%s]", rsp_code.c_str(), rsp_msg.c_str());
        return -1;
    }
    string balance = map_return["Amount"];
    PRINTF_DEBUG("Amount[%s]", balance.c_str());
    return 0;
}

int redeem() {
    string order_id = to_string(get_time_sec());
    map<string, string> map_data;
    map_data["Amount"] = g_amount;
    // map_data["BillAmount"] = g_card_amount;
    map_data["InvoiceNumber"]  = order_id;
    map_data["IdempotencyKey"] = "abcdefghi";
    map_data["CardNumber"]     = g_card_no;
    map_data["CardPIN"]        = g_card_pin;
    map_data["Notes"]          = "desc";
    string post_string         = move(map_to_json(map_data));
    // PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
    string url                 = "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/redeem";
    vector<string> vecHeadInfo = g_vecHeadInfo;
    string transaction_id      = string("TransactionId: ") + order_id;
    vecHeadInfo.push_back(transaction_id);
    string current_batch_no = string("CurrentBatchNumber: ") + g_batch_no;
    vecHeadInfo.push_back(current_batch_no);
    vecHeadInfo.push_back("Content-Type: application/json");
    int err_code;
    string err_msg;
    string rsp_string;
    int ret = http_proc(url, 10, &vecHeadInfo, post_string, rsp_string, err_code, err_msg);
    if (ret != 0) {
        PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
        return -1;
    }
    // PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
    map<string, string> map_return = move(json_to_map(rsp_string));
    g_OriginalInvoiceNumber        = order_id;
    g_OriginalBatchNumber          = g_batch_no;
    g_OriginalTransactionId        = order_id;
    string rsp_code                = map_return["ResponseCode"];
    string rsp_msg                 = map_return["ResponseMessage"];
    if (rsp_code != "0") {
        PRINTF_ERROR("rsp_code[%s] rsp_msg[%s]", rsp_code.c_str(), rsp_msg.c_str());
        return -1;
    }
    PRINTF_DEBUG("redeem %s success", g_amount.c_str());
    return 0;
}

int cancel_redeem() {
    string order_id = to_string(get_time_sec());
    map<string, string> map_data;
    map_data["CardNumber"]            = g_card_no;
    map_data["OriginalInvoiceNumber"] = g_OriginalInvoiceNumber;
    map_data["OriginalAmount"]        = g_amount;
    map_data["OriginalBatchNumber"]   = g_OriginalBatchNumber;
    map_data["OriginalTransactionId"] = g_OriginalTransactionId;
    string post_string                = move(map_to_json(map_data));
    // PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
    string url =
        "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/"
        "cancelredeem";
    vector<string> vecHeadInfo = g_vecHeadInfo;
    string transaction_id      = string("TransactionId: ") + order_id;
    vecHeadInfo.push_back(transaction_id);
    string current_batch_no = string("CurrentBatchNumber: ") + g_batch_no;
    vecHeadInfo.push_back(current_batch_no);
    vecHeadInfo.push_back("Content-Type: application/json");
    int err_code;
    string err_msg;
    string rsp_string;
    int ret = http_proc(url, 10, &vecHeadInfo, post_string, rsp_string, err_code, err_msg);
    if (ret != 0) {
        PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
        return -1;
    }
    // PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
    map<string, string> map_return = move(json_to_map(rsp_string));
    string rsp_code                = map_return["ResponseCode"];
    string rsp_msg                 = map_return["ResponseMessage"];
    if (rsp_code != "0") {
        PRINTF_ERROR("rsp_code[%s] rsp_msg[%s]", rsp_code.c_str(), rsp_msg.c_str());
        return -1;
    }
    PRINTF_DEBUG("cancel redeem %s success", g_amount.c_str());
    return 0;
}

int reverse_redeem() {
    string order_id = to_string(get_time_sec());
    map<string, string> map_data;
    map_data["CardNumber"]    = g_card_no;
    map_data["InvoiceNumber"] = order_id;
    string post_string        = move(map_to_json(map_data));
    // PRINTF_DEBUG("post_string:[%s]", post_string.c_str());
    string url =
        "https://qc3.qwikcilver.com/QwikCilver/eGMS.RestAPI/api/gc/"
        "reverseredeem";
    vector<string> vecHeadInfo = g_vecHeadInfo;
    string transaction_id      = string("TransactionId: ") + order_id;
    vecHeadInfo.push_back(transaction_id);
    string current_batch_no = string("CurrentBatchNumber: ") + g_batch_no;
    vecHeadInfo.push_back(current_batch_no);
    vecHeadInfo.push_back("Content-Type: application/json");
    int err_code;
    string err_msg;
    string rsp_string;
    int ret = http_proc(url, 10, &vecHeadInfo, post_string, rsp_string, err_code, err_msg);
    if (ret != 0) {
        PRINTF_ERROR("err_code:[%d] err_msg:[%s]", err_code, err_msg.c_str());
        return -1;
    }
    // PRINTF_DEBUG("rsp_string:[%s]", rsp_string.c_str());
    map<string, string> map_return = move(json_to_map(rsp_string));
    string rsp_code                = map_return["ResponseCode"];
    string rsp_msg                 = map_return["ResponseMessage"];
    if (rsp_code != "0") {
        PRINTF_ERROR("rsp_code[%s] rsp_msg[%s]", rsp_code.c_str(), rsp_msg.c_str());
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    init();

    // RETURN_ON_ERROR(get_batch_no(g_batch_no));
    g_batch_no = "10648684";

    RETURN_ON_ERROR(balance_enquiry());

    RETURN_ON_ERROR(redeem());

    RETURN_ON_ERROR(balance_enquiry());

    RETURN_ON_ERROR(cancel_redeem());
    // RETURN_ON_ERROR(reverse_redeem());

    RETURN_ON_ERROR(balance_enquiry());
    return 0;
}