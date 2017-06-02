#pragma once
#include "promise/Promise.h"
#include <memory>

namespace http
{
class HttpRequest;
using HttpRequestSharedPtr = std::shared_ptr<HttpRequest>;

class IHttpResponse;
using IHttpResponseSharedPtr = std::shared_ptr<IHttpResponse>;

using HttpPromise = isc::Promise<IHttpResponseSharedPtr>;

class IAsyncHttpClient
{
public:
	virtual ~IAsyncHttpClient() = default;

	virtual HttpPromise Send(const HttpRequestSharedPtr& request) = 0;
};
}
