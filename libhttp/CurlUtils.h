#pragma once

namespace http
{
class CurlUtils
{
public:
	CurlUtils() = delete;

	// convert UTF-8 string to wstring
	static std::wstring utf8_to_wstring(std::string_view str);

	// convert wstring to UTF-8 string
	static std::string wstring_to_utf8(std::wstring_view str);
};
}
