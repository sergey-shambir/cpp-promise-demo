#include "stdafx.h"
#include "HttpRequest.h"

namespace http
{

void HttpRequest::SetUrl(std::wstring_view value)
{
	m_url = value;
}

void HttpRequest::AddHeader(std::wstring_view value)
{
	m_headers.add(value);
}

void HttpRequest::SetRequestType(RequestType value)
{
	m_type = value;
}

void HttpRequest::SetRequestData(std::string data)
{
	m_data = std::move(data);
}

}
