#include "stdafx.h"
#include "CurlUtils.h"
#include <codecvt>

namespace http
{

// convert UTF-8 string to wstring
std::wstring CurlUtils::utf8_to_wstring(std::string_view str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str.data(), str.data() + str.size());
}

// convert wstring to UTF-8 string
std::string CurlUtils::wstring_to_utf8(std::wstring_view str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str.data(), str.data() + str.size());
}

} // namespace http
