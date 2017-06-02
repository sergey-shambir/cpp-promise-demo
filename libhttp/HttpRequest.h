#pragma once
#include <curl_header.h>
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
	HttpRequest() = default;
	HttpRequest(const HttpRequest&) = delete;
	HttpRequest& operator=(const HttpRequest&) = delete;

	HttpRequest& SetUrl(std::wstring_view value);
	HttpRequest& AddHeader(std::wstring_view value);
	HttpRequest& SetRequestType(RequestType value);
	HttpRequest& SetRequestData(std::string data);

	const std::wstring& GetUrl() const;
	const curl_slist* GetHeaders() const;
	RequestType GetRequestType() const;
	const std::string& GetData() const;

private:
	std::wstring m_url;
	curl::curl_header m_headers;
	RequestType m_type = RequestType::Get;
	std::string m_data;
};

} // namespace http
