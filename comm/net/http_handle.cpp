#include "http_handle.h"
#include "printf_utils.h"

size_t recv_proc(void* ptr, size_t size, size_t nmemb, std::string* p) {

	if (p == NULL)
		return 0;

	if (size * nmemb == 0)
		return 0;

	std::string temp((char*)ptr, size * nmemb);
	p->append(temp);
	return (size * nmemb);
}

int http_proc(const std::string& strUrl, 
		unsigned iTimeout, 
		std::vector<std::string>* pvecHeadInfo,
		std::string& strPostData, 
		std::string& strRecvBuf,
		int& iResult,
		std::string& strErrmsg,
		const std::string& strCookie,
		//const std::string& strDnsList,
		//const std::string& strIpv4,
		bool https_verify) {

	int ret         =  0;
	char cerr[1024] =  {0};

	/* comment, gerryyang
	 * You are strongly advised to not allow this automatic behaviour, by calling curl_global_init(3) yourself properly
	 * 官方推荐使用curl_global_init和curl_global_cleanup
	 * */
	static bool bInit = false;
	if (bInit == false) {
		bInit = true;
		curl_global_init(CURL_GLOBAL_ALL);  
	}

	CURL* curl = curl_easy_init();
	if (!curl)  {
		PRINTF_ERROR("[curl]curl_easy_init error");
		strErrmsg = cerr;
		return -501;
	}

	struct curl_slist* headerlist =  NULL;
	if(pvecHeadInfo != NULL) {
		for(unsigned int i = 0; i < pvecHeadInfo->size(); i++) {
			headerlist = curl_slist_append(headerlist, (*pvecHeadInfo)[i].c_str());
			PRINTF_DEBUG("%s", (*pvecHeadInfo)[i].c_str());
		}
	}
	else  {
		headerlist = curl_slist_append(headerlist, "Accept: */*");
		headerlist = curl_slist_append(headerlist, "Pragma: no-cache");
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	
	if (!strCookie.empty())
		curl_easy_setopt(curl, CURLOPT_COOKIE, strCookie.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());

	if (!strPostData.empty()) {
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPostData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strPostData.size());
	}

	curl_easy_setopt(curl,   CURLOPT_WRITEFUNCTION,   recv_proc);
	curl_easy_setopt(curl,   CURLOPT_WRITEDATA,       &strRecvBuf);

//Added in cURL 7.33.0暂时不支持
/*
	if (!strDnsList.empty()) {
		curl_easy_setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, false);
		curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, strDnsList.c_str());
	}

	if (!strIpv4.empty()) {
		curl_easy_setopt(curl, CURLOPT_DNS_LOCAL_IP4, strIpv4.c_str());
	}
*/

	//设定为不验证证书和HOST
	if (!https_verify) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}

	/* comment, gerryyang
	 * In unix-like systems, CURLOPT_TIMEOUT might cause signals to be used unless CURLOPT_NOSIGNAL is set.
	 * CURLOPT_NOSIGNAL, Pass a long. If it is 1, libcurl will not use any functions that install signal handlers or any functions that cause signals to be sent to the process. This option is mainly here to allow multi-threaded unix applications to still set/use all timeout options etc, without risking getting signals. The default value for this parameter is 0. (Added in 7.10) 
	 * 注意signal在libcurl不是线程安全的, 故需要忽略所有的信号处理
	 * */
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	curl_easy_setopt(curl,   CURLOPT_CONNECTTIMEOUT,   iTimeout);
	curl_easy_setopt(curl,   CURLOPT_TIMEOUT,          iTimeout);


	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)  {
		ret = -502;

		PRINTF_ERROR("[curl]%s(%d)", curl_easy_strerror(res), res);
		strErrmsg = cerr;
		switch (res) {
			case CURLE_URL_MALFORMAT:
				;
			case CURLE_COULDNT_RESOLVE_HOST:
				;
			case CURLE_COULDNT_CONNECT: 
				iResult = -2;  
				break;
			default:
				break;
		}
	}
	else {
		long iRet;
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &iRet);
		if (res != CURLE_OK)  {
			ret = -503;
			PRINTF_ERROR("[curl]%s(%d)", curl_easy_strerror(res), res);
			strErrmsg = cerr;
		}
		else if (iRet != 200) {
			ret = -504;
			PRINTF_ERROR("[curl]Http Response FAIL(%ld)", iRet);
			strErrmsg = cerr;
		}
	}

	curl_slist_free_all(headerlist);

	/* comment, gerryyang
	 * You are strongly advised to not allow this automatic behaviour, by calling curl_global_init(3) yourself properly
	 * 官方推荐使用curl_global_init和curl_global_cleanup
	 * */
	curl_easy_cleanup(curl);

	return ret;
}

int http_proc(const std::string& strUrl, 
		unsigned iTimeout, 
		std::string& strUserPwd, // The format of which is: [user name]:[password].
		std::vector<std::string>* pvecHeadInfo,
		std::string& strPostData, 
		std::string& strRecvBuf,
		int& iResult,
		std::string& strErrmsg,
		const std::string& strCookie,
		//const std::string& strDnsList,
		//const std::string& strIpv4,
		bool https_verify) {

	int ret         =  0;
	char cerr[1024] =  {0};

	/* comment, gerryyang
	 * You are strongly advised to not allow this automatic behaviour, by calling curl_global_init(3) yourself properly
	 * 官方推荐使用curl_global_init和curl_global_cleanup
	 * */
	static bool bInit = false;
	if (bInit == false) {
		bInit = true;
		curl_global_init(CURL_GLOBAL_ALL);  
	}

	CURL* curl = curl_easy_init();
	if (!curl)  {
		PRINTF_ERROR("[curl]curl_easy_init error");
		strErrmsg = cerr;
		return -501;
	}

	struct curl_slist* headerlist =  NULL;
	if(pvecHeadInfo != NULL) {
		for(unsigned int i = 0; i < pvecHeadInfo->size(); i++) {
			headerlist = curl_slist_append(headerlist, (*pvecHeadInfo)[i].c_str());
		}
	}
	else  {
		headerlist = curl_slist_append(headerlist, "Accept: */*");
		headerlist = curl_slist_append(headerlist, "Pragma: no-cache");
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	//设置帐号和密码
	curl_easy_setopt(curl, CURLOPT_USERPWD, strUserPwd.c_str());

	if (!strCookie.empty())
		curl_easy_setopt(curl, CURLOPT_COOKIE, strCookie.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());

	if (strPostData != "") {
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPostData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strPostData.size());
	}

	curl_easy_setopt(curl,   CURLOPT_WRITEFUNCTION,   recv_proc);
	curl_easy_setopt(curl,   CURLOPT_WRITEDATA,       &strRecvBuf);

//Added in cURL 7.33.0暂时不支持
/*
	if (!strDnsList.empty()) {
		curl_easy_setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, false);
		curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, strDnsList.c_str());
	}

	if (!strIpv4.empty()) {
		curl_easy_setopt(curl, CURLOPT_DNS_LOCAL_IP4, strIpv4.c_str());
	}
*/

	//设定为不验证证书和HOST
	if (!https_verify) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}

	/* comment, gerryyang
	 * In unix-like systems, CURLOPT_TIMEOUT might cause signals to be used unless CURLOPT_NOSIGNAL is set.
	 * CURLOPT_NOSIGNAL, Pass a long. If it is 1, libcurl will not use any functions that install signal handlers or any functions that cause signals to be sent to the process. This option is mainly here to allow multi-threaded unix applications to still set/use all timeout options etc, without risking getting signals. The default value for this parameter is 0. (Added in 7.10) 
	 * 注意signal在libcurl不是线程安全的, 故需要忽略所有的信号处理
	 * */
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	curl_easy_setopt(curl,   CURLOPT_CONNECTTIMEOUT,   iTimeout);
	curl_easy_setopt(curl,   CURLOPT_TIMEOUT,          iTimeout);


	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)  {
		ret = -502;

		PRINTF_ERROR("[curl]%s(%d)", curl_easy_strerror(res), res);
		strErrmsg = cerr;
		switch (res) {
			case CURLE_URL_MALFORMAT:
				;
			case CURLE_COULDNT_RESOLVE_HOST:
				;
			case CURLE_COULDNT_CONNECT: 
				iResult = -2;  
				break;
			default:
				break;
		}
	}
	else {
		long iRet;
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &iRet);
		if (res != CURLE_OK)  {
			ret = -503;
			PRINTF_ERROR("[curl]%s(%d)", curl_easy_strerror(res), res);
			strErrmsg = cerr;
		}
		else if (iRet != 200) {
			ret = -504;
			PRINTF_ERROR("[curl]Http Response FAIL(%ld)", iRet);
			strErrmsg = cerr;
		}
	}

	curl_slist_free_all(headerlist);

	/* comment, gerryyang
	 * You are strongly advised to not allow this automatic behaviour, by calling curl_global_init(3) yourself properly
	 * 官方推荐使用curl_global_init和curl_global_cleanup
	 * */
	curl_easy_cleanup(curl);

	return ret;
}
