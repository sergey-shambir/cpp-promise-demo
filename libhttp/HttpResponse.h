#pragma once
#include "IHttpResponse.h"
#include <curl/curl.h>
#include <regex>

namespace http
{

class HttpResponse : public IHttpResponse
{
public:
	using HeaderPair = std::pair<std::wstring, std::wstring>;

	HttpResponse();

	void AppendData(std::string_view chunk);
	void AppendHeader(std::string_view header);
	void SetStatusCode(int value);
	char* GetErrorBuffer();

	int GetStatusCode() const override;
	std::string_view GetData() const override;
	std::wstring GetHeaderValue(const std::wstring& key) const override;

private:
	std::wregex m_headerRe;
	std::vector<HeaderPair> m_headers;
	std::string m_data;
	int m_statusCode = 0;
	char m_errorBuffer[CURL_ERROR_SIZE];
};

} // namespace http
