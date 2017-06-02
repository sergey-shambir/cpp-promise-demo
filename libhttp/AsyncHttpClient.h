#pragma once
#include "IAsyncHttpClient.h"
#include "JoinableThread.h"
#include <condition_variable>
#include <curl_global.h>
#include <curl_multi.h>
#include <deque>
#include <mutex>

namespace http
{
class HttpRequest;
class HttpResponse;
class HttpResponsePromiseObject;

class AsyncHttpClient : public IAsyncHttpClient
{
public:
	AsyncHttpClient(const isc::IDispatcherPtr& dispatcher);
	~AsyncHttpClient();

	HttpPromise Send(const HttpRequestSharedPtr& request) override;

private:
	using HttpPromiseObjectPtr = std::shared_ptr<HttpResponsePromiseObject>;
	using HttpResponseSharedPtr = std::shared_ptr<HttpResponse>;

	struct PendingRequestData
	{
		HttpPromiseObjectPtr promiseObj;
		HttpRequestSharedPtr request;
	};

	struct ActiveRequestData
	{
		ActiveRequestData(std::unique_ptr<curl::curl_easy> handle, HttpPromiseObjectPtr promiseObj, HttpRequestSharedPtr request, HttpResponseSharedPtr response)
			: handle(std::move(handle))
			, promiseObj(std::move(promiseObj))
			, request(std::move(request))
			, response(std::move(response))
		{
		}

		std::unique_ptr<curl::curl_easy> handle;
		HttpPromiseObjectPtr promiseObj;
		HttpRequestSharedPtr request;
		HttpResponseSharedPtr response;
	};

	enum class State
	{
		NotStarted,
		Working,
		Shutdown,
		ThrownException,
	};
	bool LazyInitNetworkThread();
	void NetworkThread();
	bool TrySpawnRequest(curl::curl_multi& multiHandle);
	void DispatchCompletedRequests(curl::curl_multi& multiHandle);
	void RemoveCancelledActiveRequests(curl::curl_multi& multiHandle);
	void RejectAll(curl::curl_multi& curlMulti);
	void WaitWhileNoRequests();

	curl::curl_global m_curlInit;

	std::vector<ActiveRequestData> m_activeRequests;

	std::weak_ptr<isc::IDispatcher> m_callbackDispatcher;

	std::atomic<State> m_workingThreadState = ATOMIC_VAR_INIT(State::NotStarted);
	JoinableThread<std::thread> m_workingThread;

	std::mutex m_mutex;
	std::deque<PendingRequestData> m_requestsQueue;
	std::condition_variable_any m_postedRequest;
	std::exception_ptr m_exception;
};
}
