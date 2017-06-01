#pragma once
#include <curl/curl.h>
#include <string_view>
#include <system_error>

namespace curl
{
namespace detail
{
struct easy_deleter
{
	void operator()(CURL*);
};
struct multi_deleter
{
	void operator()(CURLM*);
};
}

using easy_ptr = std::unique_ptr<CURL, detail::easy_deleter>;
using multi_ptr = std::unique_ptr<CURLM, detail::multi_deleter>;

easy_ptr make_easy();
multi_ptr make_multi();

const std::error_category& get_curl_category();
void throw_if_failed(CURLcode, std::string_view where);

class curl_initializer
{
public:
	curl_initializer();
	~curl_initializer();

	curl_initializer(const curl_initializer&) = delete;
	curl_initializer& operator=(const curl_initializer&) = delete;
};

class header_list
{
public:
	~header_list();

	void add(std::wstring_view header);

	const curl_slist* get() const;

private:
	curl_slist* m_head = nullptr;
};
}
