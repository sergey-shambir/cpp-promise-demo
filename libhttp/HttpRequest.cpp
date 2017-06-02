#include "stdafx.h"
#include "HttpRequest.h"
#include "CurlUtils.h"

namespace http
{

const std::wstring& HttpRequest::GetUrl() const
{
	return m_url;
}

const curl_slist* HttpRequest::GetHeaders() const
{
	return m_headers.get();
}

http::RequestType HttpRequest::GetRequestType() const
{
	return m_type;
}

const std::string& HttpRequest::GetData() const
{
	return m_data;
}

HttpRequest& HttpRequest::SetUrl(std::wstring_view value)
{
	m_url = value;
	return *this;
}

HttpRequest& HttpRequest::AddHeader(std::wstring_view value)
{
	m_headers.add(CurlUtils::wstring_to_utf8(value));
	return *this;
}

HttpRequest& HttpRequest::SetRequestType(RequestType value)
{
	m_type = value;
	return *this;
}

HttpRequest& HttpRequest::SetRequestData(std::string data)
{
	m_data = std::move(data);
	return *this;
}
}
