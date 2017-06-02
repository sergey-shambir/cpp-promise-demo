#pragma once
#include <curl_easy.h>

namespace http
{
using curl_easy_ptr = std::unique_ptr<curl::curl_easy>;

class HttpResponse;
class HttpRequest;

class CurlUtils
{
public:
	CurlUtils() = delete;

	// convert UTF-8 string to wstring
	static std::wstring utf8_to_wstring(std::string_view str);

	// convert wstring to UTF-8 string
	static std::string wstring_to_utf8(std::wstring_view str);

	static curl_easy_ptr MakeCurlEasy(const HttpRequest& request, HttpResponse& response);
};
}
