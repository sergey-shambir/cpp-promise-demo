#pragma once
#include "CurlRAII.h"
#include <string_view>

namespace http
{
enum class RequestType
{
	Get,
	Post,
	Put,
	Delete,
};

class HttpRequest
{
public:
	void SetUrl(std::wstring_view value);
	void AddHeader(std::wstring_view value);
	void SetRequestType(RequestType value);
	void SetRequestData(std::string data);

private:
	std::wstring m_url;
	curl::header_list m_headers;
	RequestType m_type = RequestType::Get;
	std::string m_data;
};

} // namespace http
