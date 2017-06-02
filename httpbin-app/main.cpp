#include "AsyncHttpClient.h"
#include "HttpRequest.h"
#include "IHttpResponse.h"
#include "platform/AsioEventLoop.h"
#include <iostream>

void consume(const http::HttpPromise& promise, const isc::Operation& callback)
{
	promise.Then([callback](auto response) {
		std::cout << "status code: " << response->GetStatusCode() << std::endl;
		std::cout << "response: " << response->GetData() << std::endl;
		callback();
	});
	promise.Catch([callback](const std::exception_ptr& exception) {
		try
		{
			std::rethrow_exception(exception);
		}
		catch (const std::exception& ex)
		{
			std::cout << "error: " << ex.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "unknown error" << std::endl;
		}
		callback();
	});
}

int main()
{
	auto eventLoop = std::make_shared<isc::AsioEventLoop>();
	http::AsyncHttpClient httpClient(eventLoop);

	auto request1 = std::make_shared<http::HttpRequest>();
	request1->SetUrl(L"http://httpbin.org/ip");
	auto promise1 = httpClient.Send(request1);

	auto request2 = std::make_shared<http::HttpRequest>();
	request2->SetUrl(L"http://httpbin.org/redirect-to?url=status%2F404");
	auto promise2 = httpClient.Send(request2);

	auto request3 = std::make_shared<http::HttpRequest>();
	request3->SetUrl(L"http://httpbin.org/ip");
	auto promise3 = httpClient.Send(request3);

	auto request4 = std::make_shared<http::HttpRequest>();
	request4->SetUrl(L"http://notexist.example.com");
	auto promise4 = httpClient.Send(request4);

	int taskCount = 4;
	auto onTaskDone = [&]() {
		--taskCount;
		if (taskCount == 0)
		{
			eventLoop->DeferQuit();
		}
	};

	consume(promise1, onTaskDone);
	consume(promise2, onTaskDone);
	consume(promise3, onTaskDone);
	consume(promise4, onTaskDone);

	eventLoop->Run();

	return 0;
}
