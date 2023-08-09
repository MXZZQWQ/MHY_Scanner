﻿#include <iomanip>
#include <chrono>
#include <sstream>
#include "HttpClient.h"


std::string HttpClient::string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	memset(pwBuf, 0, (nwLen + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	return retStr;
}

std::string HttpClient::UTF8_To_string(const std::string& str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, (nwLen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, 0, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[] pBuf;
	delete[] pwBuf;

	return retStr;
}

std::string HttpClient::Url(std::string url, std::map<std::string, std::string> params)
{
	std::ostringstream paramsTemp;
	bool first = true;
	for (const auto& kv : params)
	{
		if (!first)
		{
			paramsTemp << "&";
		}
		first = false;
		paramsTemp << kv.first << "=" << kv.second;
	}
	return url + "?" + paramsTemp.str();//url拼接完成
}

size_t HttpClient::req_reply(void* ptr, size_t size, size_t nmemb, void* stream)//get请求和post请求数据响应函数
{
	//std::cout << "----->reply" << std::endl;
	std::string* str = (std::string*)stream;
	//cout << *str << endl;
	(*str).append((char*)ptr, size * nmemb);
	return size * nmemb;
}

CURLcode HttpClient::GetRequest(std::string& response, const std::string& url, std::map<std::string, std::string> headers)
{
	CURL* curl = curl_easy_init();
	CURLcode res{};
	if (curl)
	{
		struct curl_slist* headerList = NULL;
		for (const auto& kv : headers)
		{
			std::string header = kv.first + ": " + kv.second;
			headerList = curl_slist_append(headerList, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // 设置请求的 URL
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // 设置回调函数，保存请求返回的数据
		res = curl_easy_perform(curl); // 发送请求
		/* always cleanup */
		curl_easy_cleanup(curl);
		curl_slist_free_all(headerList);
	}
	return res;
}

CURLcode HttpClient::PostRequest(std::string& response, const std::string& url, const std::string& postParams, std::map<std::string, std::string> headers)
{
	CURL* curl = curl_easy_init();
	CURLcode res{};
	if (curl)
	{
		struct curl_slist* headerList = NULL;
		for (const auto& kv : headers)
		{
			std::string header = kv.first + ": " + kv.second;
			headerList = curl_slist_append(headerList, header.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

		//不接收响应头数据0代表不接收 1代表接收
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		//设置请求为post请求
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		//设置请求的URL地址
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//设置post请求的参数
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str());
		//设置ssl验证
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		//CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		//设置数据接收和写入函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		//设置超时时间(单位：s)
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
		// 开启post请求
		res = curl_easy_perform(curl);
		//释放curl 
		curl_easy_cleanup(curl);
		curl_slist_free_all(headerList);
	}
	return res;
}

int HttpClient::getCurrentUnixTime()const
{
	return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

std::string HttpClient::urlEncode(const std::string& str)
{
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (auto itr = str.begin(), end = str.end(); itr != end; ++itr)
	{
		const unsigned char c = *itr;

		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'
			|| c == '!' || c == '*' || c == '\'' || c == '(' || c == ')'
			|| c == ';' || c == ':' || c == '@' || c == '&' || c == '='
			|| c == '$' || c == ',' || c == '/' || c == '?'
			|| c == '#' || c == '[' || c == ']')
		{
			escaped << c;
			continue;
		}

		escaped << std::uppercase;
		escaped << '%' << std::setw(2) << int((unsigned char)c);
		escaped << std::nouppercase;
	}

	return escaped.str();
}

std::string HttpClient::replaceQuotes(const std::string& str)
{
	std::string newStr;
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\"')
		{
			newStr += '\\';
		}
		newStr += str[i];
	}
	return newStr;
}