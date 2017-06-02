#pragma once
#include <string>
#include <string_view>

namespace http
{

class IHttpResponse
{
public:
	virtual ~IHttpResponse() = default;

	// Returns HTTP status code.
	virtual int GetStatusCode() const = 0;

	// Returns HTTP response data.
	virtual std::string_view GetData() const = 0;

	// Returns value for given header.
	virtual std::wstring GetHeaderValue(const std::wstring& key) const = 0;
};

} // namespace http
