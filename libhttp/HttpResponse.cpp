#include "stdafx.h"
#include "HttpResponse.h"
#include "CurlUtils.h"
#include <algorithm>

namespace http
{

void HttpResponse::AppendData(std::string_view chunk)
{
	m_data.append(chunk);
}

void HttpResponse::AppendHeader(std::string_view key, std::string_view value)
{
	m_headers.emplace_back(CurlUtils::utf8_to_wstring(key), CurlUtils::utf8_to_wstring(value));
}

void HttpResponse::SetStatusCode(int value)
{
	m_statusCode = value;
}

char* HttpResponse::GetErrorBuffer()
{
	return m_erroBuffer;
}

int HttpResponse::GetStatusCode() const
{
	return m_statusCode;
}

std::string_view HttpResponse::GetData() const
{
	return m_data;
}

std::wstring HttpResponse::GetHeaderValue(const std::wstring &key) const
{
	auto it = std::find_if(m_headers.begin(), m_headers.end(), [&](const auto& pair) {
		return (pair.first == key);
	});
	if (it != m_headers.end())
	{
		return it->second;
	}
	return std::wstring();
}

}
