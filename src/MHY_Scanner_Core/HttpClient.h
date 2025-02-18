﻿#pragma once

#include <string>
#include <map>
#include <iostream>

#include <curl/curl.h>

#include "Common.h"

class HttpClient
{
public:
	CURLcode GetRequest(std::string& response, const char* url, std::map<std::string, std::string> headers = {});
	CURLcode PostRequest(std::string& response, const char* url, const std::string& postParams, std::map<std::string, std::string> headers = {});
	std::string MapToQueryString(const std::map<std::string, std::string>& params);
	std::map<std::string, std::string> QueryStringToMap(const std::string& str);
	int getCurrentUnixTime()const;
	[[nodiscard]] std::string urlEncode(const std::string& str);
	[[nodiscard]] std::string urlDecode(const std::string& str);
	[[nodiscard]] std::string UTF8_To_string(const std::string& str);
	[[nodiscard]] std::string string_To_UTF8(const std::string& str);
	std::string replaceQuotes(const std::string& str);
private:
	static size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream);
};