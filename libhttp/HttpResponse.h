#pragma once
#include "IHttpResponse.h"
#include <curl/curl.h>

namespace http
{

class HttpResponse : public IHttpResponse
{
public:
	using HeaderPair = std::pair<std::wstring, std::wstring>;

	void AppendData(std::string_view chunk);
	void AppendHeader(std::string_view key, std::string_view value);
	void SetStatusCode(int value);
	char* GetErrorBuffer();

	int GetStatusCode() const override;
	std::string_view GetData() const override;
	std::wstring GetHeaderValue(const std::wstring& key) const override;

private:
	std::vector<HeaderPair> m_headers;
	std::string m_data;
	int m_statusCode = 0;
	char m_erroBuffer[CURL_ERROR_SIZE];
};

} // namespace http
