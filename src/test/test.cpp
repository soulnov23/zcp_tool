#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "oauth.h"
#include "src/base/coder.h"
#include "src/base/log.h"
#include "src/base/string_util.h"
#include "src/base/time_util.h"

extern "C" {
#include "oauth.h"
}

#ifndef SIGCHECK_ERROR
#define SIGCHECK_ERROR(fmt, args...) \
    snprintf(m_szErrMsg, sizeof(m_szErrMsg), "[%s][%d][%s]" fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#endif

namespace NS_SIG_CHECK {

    struct StSigInfo {
        string method;  // GET or POST
        string url_path;
        map<string, string> params;

        string share_secret;

        string desc() {
            stringstream ss;
            // ss << endl;
            ss << "share_secret:" << share_secret;  //<< endl;
            ss << " method:" << method;             // << endl;
            ss << " url_path:" << url_path;         //<< endl;

            ss << " params:";  //<< endl;

            for (map<string, string>::iterator it = params.begin(); it != params.end(); ++it) {
                ss << it->first << ":" << it->second << " ";  //<< endl;
            }

            return ss.str();
        }
    };

    class CSigCheck {
    public:
        CSigCheck() {}
        virtual ~CSigCheck() {}

        const char* GetErrMsg() { return m_szErrMsg; }

        string Create(StSigInfo& data) {
            string source;

            //转化为大写
            transform(data.method.begin(), data.method.end(), data.method.begin(), ::toupper);

            source.append(data.method);
            source.append("&");
            source.append(url_encode(data.url_path));
            source.append("&");
            source.append(url_encode(join_params(data.params)));

            char* p_sig =
                oauth_sign_hmac_sha1_raw(source.c_str(), source.size(), data.share_secret.c_str(), data.share_secret.size());

            if (p_sig == NULL) {
                return "";
            }

            string sig = p_sig;
            ;

            delete[] p_sig;
            p_sig = NULL;

            return sig;
        }

        int Check(StSigInfo& data, const string& param_sig) {
            if (param_sig.empty()) {
                SIGCHECK_ERROR("param_sig is empty");
                return -1;
            }

            string real_sig = Create(data);

            if (real_sig != param_sig) {
                SIGCHECK_ERROR("sig not match,param_sig[%s], real_sig[%s], sig_info[%s]", url_encode(param_sig).c_str(),
                               url_encode(real_sig).c_str(), url_encode(data.desc()).c_str());
                return -2;
            }

            return 0;
        }

    private:
        string url_encode(const string& src) {
            char* p_dest = oauth_url_escape(src.c_str());
            if (p_dest == NULL) {
                return "";
            }
            string str_dest = p_dest;

            delete[] p_dest;
            p_dest = NULL;

            str_dest = replace_str(str_dest, "~", "%7E");

            return str_dest;
        }

        string join_params(map<string, string>& params) {
            string source;
            for (map<string, string>::iterator it = params.begin(); it != params.end(); ++it) {
                if (it != params.begin()) {
                    source.append("&");
                }
                source.append(it->first);
                source.append("=");
                source.append(it->second);
            }

            return source;
        }

        string replace_str(string src, const string& old_value, const string& new_value) {
            for (string::size_type pos(0); pos != string::npos; pos += new_value.size()) {
                if ((pos = src.find(old_value, pos)) != string::npos) {
                    src.replace(pos, old_value.size(), new_value);
                } else {
                    break;
                }
            }

            return src;
        }

    private:
        char m_szErrMsg[1024];
    };

}  // namespace NS_SIG_CHECK

int main(int argc, char* argv[]) {
    map<string, string> order_params;
    order_params["offer_id"] = "1450015065";
    order_params["overseas_cmd"] = "order";
    order_params["pf"] = "xxx-yyy-android";
    order_params["pfkey"] = "pfkey";
    order_params["from_https"] = "1";
    order_params["openkey"] = "nokey";
    order_params["openid"] = "test123456";
    order_params["session_id"] = "hy_gameid";
    order_params["session_type"] = "st_dummy";
    order_params["mid"] = "1";
    order_params["ver"] = "v1";
    order_params["buytype"] = "uni_pdt_v2";
    order_params["pay_method"] = "uniacct";
    order_params["zoneid"] = "1";
    order_params["country"] = "US";
    order_params["currency_type"] = "USD";
    order_params["req_from"] = "backend";
    order_params["provide_type"] = "uni_pdt_v2";
    order_params["ts"] = to_string(get_time_sec());

    // 计算签名
    NS_SIG_CHECK::StSigInfo data;
    NS_SIG_CHECK::CSigCheck c;
    string secretkey = "ElJpeCnJuZHsXnf9rTqerPuPTxJG4vin&";

    data.params = order_params;
    data.share_secret = secretkey;
    data.method = "GET";
    data.url_path = "/cgi-bin/mobile_overseas_order.fcg";
    string sig = c.Create(data);
    LOG_DEBUG("sign_sign=[{}] appkey=[{}]", sig, secretkey);

    string request_str;
    map2str(request_str, order_params);
    string encode_sig;
    url_encode(sig, encode_sig);
    request_str += "&sig=" + encode_sig;
    LOG_DEBUG("cgi order request[http://172.24.0.150:8880/cgi-bin/mobile_overseas_order.fcg?{}]", request_str);
    return 0;
}