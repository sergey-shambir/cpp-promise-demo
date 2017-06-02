#include "stdafx.h"
#include "HttpResponse.h"
#include "CurlUtils.h"
#include <algorithm>

namespace http
{

HttpResponse::HttpResponse()
	: m_headerRe(LR"**(([^:]+)\:\W+(.+)[\r\n]+)**")
{
	std::memset(m_errorBuffer, 0, sizeof(m_errorBuffer));
}

void HttpResponse::AppendData(std::string_view chunk)
{
	m_data.append(chunk);
}

void HttpResponse::AppendHeader(std::string_view header)
{
	const std::wstring headerUtf16 = CurlUtils::utf8_to_wstring(header);
	std::wsmatch match;
	if (std::regex_match(headerUtf16, match, m_headerRe) && (match.size() >= 3))
	{
		const std::wstring key = match[1];
		const std::wstring value = match[2];
		m_headers.emplace_back(key, value);
	}
}

void HttpResponse::SetStatusCode(int value)
{
	m_statusCode = value;
}

char* HttpResponse::GetErrorBuffer()
{
	return m_errorBuffer;
}

int HttpResponse::GetStatusCode() const
{
	return m_statusCode;
}

std::string_view HttpResponse::GetData() const
{
	return m_data;
}

std::wstring HttpResponse::GetHeaderValue(const std::wstring& key) const
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
