#include "stdafx.h"
#include "CurlUtils.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <codecvt>

namespace http
{
namespace
{
// Callback function used by libcurl for collect response data
size_t WriteData(void* ptr, size_t size, size_t nmemb, void* stream)
{
	char* input = reinterpret_cast<char*>(ptr);
	HttpResponse* response = reinterpret_cast<HttpResponse*>(stream);
	const size_t dataSize = size * nmemb;

	std::string_view data(input, dataSize);
	response->AppendData(data);

	return dataSize;
}

// Callback function used by libcurl for collect header data
size_t WriteHeaderData(void* ptr, size_t size, size_t nmemb, void* stream)
{
	char* input = reinterpret_cast<char*>(ptr);
	HttpResponse* response = reinterpret_cast<HttpResponse*>(stream);
	const size_t dataSize = size * nmemb;

	response->AppendHeader(std::string(input, dataSize));

	return dataSize;
}
}

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

curl_easy_ptr CurlUtils::MakeCurlEasy(const HttpRequest& request, HttpResponse& response)
{
	auto curl = std::make_unique<curl::curl_easy>();

	curl->add<CURLOPT_NOSIGNAL>(1L);
	curl->add<CURLOPT_ACCEPT_ENCODING>("");
	curl->add<CURLOPT_TRANSFER_ENCODING>(0L);

	curl->add<CURLOPT_URL>(wstring_to_utf8(request.GetUrl()).c_str());
	curl->add<CURLOPT_HTTPHEADER>(request.GetHeaders());
	curl->add<CURLOPT_WRITEFUNCTION>(WriteData);
	curl->add<CURLOPT_WRITEDATA>(&response);
	curl->add<CURLOPT_HEADERFUNCTION>(WriteHeaderData);
	curl->add<CURLOPT_HEADERDATA>(&response);

	curl->add<CURLOPT_ERRORBUFFER>(response.GetErrorBuffer());

	const auto& requestData = request.GetData();

	switch (request.GetRequestType())
	{
	case RequestType::Get:
		curl->add<CURLOPT_FOLLOWLOCATION>(1L);
		break;
	case RequestType::Delete:
		curl->add<CURLOPT_CUSTOMREQUEST>("DELETE");
		curl->add<CURLOPT_FOLLOWLOCATION>(1L);
		break;
	case RequestType::Post:
		curl->add<CURLOPT_POST>(1L);
		curl->add<CURLOPT_POSTFIELDS>(requestData.data());
		curl->add<CURLOPT_POSTFIELDSIZE>(static_cast<long>(requestData.size()));
		break;
	case RequestType::Put:
		curl->add<CURLOPT_CUSTOMREQUEST>("PUT");
		curl->add<CURLOPT_POSTFIELDS>(requestData.data());
		curl->add<CURLOPT_POSTFIELDSIZE>(static_cast<long>(requestData.size()));
		break;
	}

	return curl;
}

} // namespace http
