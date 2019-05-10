#ifndef __HTTP_HANDLE_H__
#define __HTTP_HANDLE_H__

#include "curl/curl.h"
#include <string>
#include <vector>
#include <map>

size_t recv_proc(void *ptr, size_t size, size_t nmemb, std::string *p);

int http_proc(const std::string &strUrl, 
		unsigned iTimeout, 
		std::vector<std::string> *pvecHeadInfo,
		std::map<std::string, std::string> &mapPostData, 
		std::string &strRecvBuf,
		int &iResult,
		std::string &strErrmsg,
		const std::string &strCookie = "");

int http_proc(const std::string &strUrl, 
		unsigned iTimeout, 
		std::vector<std::string> *pvecHeadInfo,
		std::string &strPostData, 
		std::string &strRecvBuf,
		int &iResult,
		std::string &strErrmsg,
		const std::string &strCookie = "");

int http_proc(const std::string &strUrl, 
		unsigned iTimeout, 
		std::string &strUserPwd, // The format of which is: [user name]:[password].
		std::vector<std::string> *pvecHeadInfo,
		std::string &strPostData, 
		std::string &strRecvBuf,
		int &iResult,
		std::string &strErrmsg,
		const std::string &strCookie = "");

#endif /* __HTTP_HANDLE_H__ */
