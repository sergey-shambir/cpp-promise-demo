#include "stdafx.h"
#include "AsyncHttpClient.h"
#include "CurlUtils.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "erase_if.h"
#include <cassert>

using unique_lock = std::unique_lock<std::mutex>;

namespace http
{
class HttpResponsePromiseObject
	: public isc::PromiseObject<IHttpResponseSharedPtr>
{
public:
	HttpResponsePromiseObject(DispatcherWeakPtr dispatcher)
		: PromiseObject(std::move(dispatcher))
	{
		DoOnCancel([this] {
			m_isCancelled = true;
		});
	}

	bool IsCancelled() const
	{
		return m_isCancelled;
	}

private:
	std::atomic_bool m_isCancelled = ATOMIC_VAR_INIT(false);
};

namespace
{
const size_t MAX_CONCURRENT_REQUESTS = 8;
const int WAIT_TIMEOUT_MS = 10;
}

AsyncHttpClient::AsyncHttpClient(const isc::IDispatcherPtr& dispatcher)
	: m_callbackDispatcher(std::move(dispatcher))
{
}

AsyncHttpClient::~AsyncHttpClient()
{
	// Report to thread procedure about shutdown.
	m_workingThreadState.exchange(State::Shutdown);

	// Wakeup thread procedure.
	m_postedRequest.notify_all();

	// Join if thread was created already.
	if (m_workingThread.joinable())
	{
		m_workingThread.join();
	}
}

http::HttpPromise AsyncHttpClient::Send(const HttpRequestSharedPtr& request)
{
	auto callbackDispatcher = m_callbackDispatcher.lock();
	if (!callbackDispatcher)
	{
		throw std::logic_error("m_callbackDispatcher is empty");
	}
	if (!request)
	{
		throw std::invalid_argument("request is invalid");
	}

	// Rethrow exception if network thread thrown previously.
	if (m_workingThreadState == State::ThrownException)
	{
		std::rethrow_exception(m_exception);
	}
	LazyInitNetworkThread();

	auto promiseObj = std::make_shared<HttpResponsePromiseObject>(callbackDispatcher);

	{
		unique_lock lock(m_mutex);
		m_requestsQueue.emplace_back(PendingRequestData{ promiseObj, request });
	}
	m_postedRequest.notify_one();

	// Rare case when exception happens after previous check.
	if (m_workingThreadState == State::ThrownException)
	{
		promiseObj->Reject(m_exception);
	}

	return HttpPromise(promiseObj);
}

bool AsyncHttpClient::LazyInitNetworkThread()
{
	State oldState = State::NotStarted;
	if (m_workingThreadState.compare_exchange_strong(oldState, State::Working) && oldState == State::NotStarted)
	{
		m_workingThread = std::thread(std::bind(&AsyncHttpClient::NetworkThread, this));
	}
	return true;
}

void AsyncHttpClient::NetworkThread()
{
	curl::curl_multi multiHandle;
	try
	{
		while (m_workingThreadState.load() != State::Shutdown)
		{
			if ((m_activeRequests.size() < MAX_CONCURRENT_REQUESTS) && TrySpawnRequest(multiHandle))
			{
				// we should perform request after spawn.
				multiHandle.perform();
			}
			else if (m_activeRequests.empty())
			{
				WaitWhileNoRequests();
			}

			if (!m_activeRequests.empty())
			{
				multiHandle.perform();
				DispatchCompletedRequests(multiHandle);
			}
			RemoveCancelledActiveRequests(multiHandle);

			// reducing the CPU load
			multiHandle.wait(nullptr, 0, WAIT_TIMEOUT_MS, nullptr);
		}
	}
	catch (...)
	{
		m_exception = std::current_exception();
		m_workingThreadState.exchange(State::ThrownException);
		RejectAll(multiHandle);
	}

	{
		unique_lock lock(m_mutex);
		m_requestsQueue.clear();
	}
	m_activeRequests.clear();
}

bool AsyncHttpClient::TrySpawnRequest(curl::curl_multi& multiHandle)
{
	while (true)
	{
		PendingRequestData pendingData;
		{
			unique_lock lock(m_mutex);
			if (m_requestsQueue.empty())
			{
				return false;
			}
			pendingData = m_requestsQueue.front();
			m_requestsQueue.pop_front();
		}

		assert(pendingData.promiseObj);
		if (!pendingData.promiseObj->IsCancelled())
		{
			auto response = std::make_shared<HttpResponse>();
			auto handle = CurlUtils::MakeCurlEasy(*pendingData.request, *response);
			multiHandle.add(*handle);
			m_activeRequests.emplace_back(std::move(handle), pendingData.promiseObj, pendingData.request, response);
			return true;
		}
	}
}

void AsyncHttpClient::DispatchCompletedRequests(curl::curl_multi& multiHandle)
{
	// Using C API to workaround for issue in curlcpp: https://github.com/JosephP91/curlcpp/issues/76

	CURLM* curlMulti = multiHandle.get_curl();
	assert(curlMulti);
	int msgsLeft = 0;
	while (auto curlMessage = curl_multi_info_read(curlMulti, &msgsLeft))
	{
		if (curlMessage->msg == CURLMSG_DONE)
		{
			auto foundIt = std::find_if(m_activeRequests.begin(), m_activeRequests.end(), [&](const auto& data) {
				return (data.handle->get_curl() == curlMessage->easy_handle);
			});
			if (foundIt != m_activeRequests.end())
			{
				ActiveRequestData& data = *foundIt;
				const long responseCode = data.handle->get_info<CURLINFO_RESPONSE_CODE>().get();

				if (responseCode == 0)
				{
					auto exception = std::make_exception_ptr(std::runtime_error(data.response->GetErrorBuffer()));
					data.promiseObj->Reject(exception);
				}
				else
				{
					data.response->SetStatusCode(responseCode);
					data.promiseObj->Resolve(std::move(data.response));
				}

				multiHandle.remove(*data.handle);
				m_activeRequests.erase(foundIt);
			}
		}
	}
}

void AsyncHttpClient::RemoveCancelledActiveRequests(curl::curl_multi& multiHandle)
{
	erase_if(m_activeRequests, [&](const auto& data) {
		if (data.promiseObj->IsCancelled())
		{
			multiHandle.remove(*data.handle);
			return true;
		}
		return false;
	});
}

void AsyncHttpClient::RejectAll(curl::curl_multi& curlMulti)
{
	// Reject all active requests.
	for (auto& activeData : m_activeRequests)
	{
		activeData.promiseObj->Reject(m_exception);
		curlMulti.remove(*activeData.handle);
	}
	m_activeRequests.clear();

	// Reject all pending requests.
	unique_lock lock(m_mutex);
	for (auto& pending : m_requestsQueue)
	{
		pending.promiseObj->Reject(m_exception);
	}
	m_requestsQueue.clear();
}

void AsyncHttpClient::WaitWhileNoRequests()
{
	unique_lock lock(m_mutex);
	m_postedRequest.wait(lock);
}
}
