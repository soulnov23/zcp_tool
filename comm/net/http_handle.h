#ifndef __HTTP_HANDLE_H__
#define __HTTP_HANDLE_H__

#include <string>
#include <vector>
#include "third_party/curl-7.29.0/include/curl/curl.h"

size_t recv_proc(void* ptr, size_t size, size_t nmemb, std::string* p);

int http_proc(const std::string& strUrl, unsigned iTimeout, std::vector<std::string>* pvecHeadInfo, std::string& strPostData,
              std::string& strRecvBuf, int& iResult, std::string& strErrmsg, const std::string& strCookie = "",
              // const std::string& strDnsList = "",
              // const std::string& strIpv4 = "",
              bool https_verify = true);

int http_proc(const std::string& strUrl, unsigned iTimeout, std::string& strUserPwd,  // The format of which is: [user
                                                                                      // name]:[password].
              std::vector<std::string>* pvecHeadInfo, std::string& strPostData, std::string& strRecvBuf, int& iResult,
              std::string& strErrmsg, const std::string& strCookie = "",
              // const std::string& strDnsList = "",
              // const std::string& strIpv4 = "",
              bool https_verify = true);

#endif /* __HTTP_HANDLE_H__ */
