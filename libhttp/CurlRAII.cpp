#include "stdafx.h"
#include "CurlRAII.h"
#include "CurlUtils.h"
#include <string>

using namespace std;

namespace curl
{
namespace detail
{
void easy_deleter::operator()(CURL* ptr)
{
	curl_easy_cleanup(ptr);
}

void multi_deleter::operator()(CURLM* ptr)
{
	curl_multi_cleanup(ptr);
}
} // namespace detail

namespace
{
class curl_category_impl : public std::error_category
{
public:
	const char* name() const noexcept override
	{
		return "curl";
	}

	std::string message(int ev) const override
	{
		return "curl error "s + curl_easy_strerror(static_cast<CURLcode>(ev));
	}
};

} // anonymous namespace

curl::easy_ptr easy_init()
{
	return curl::easy_ptr(curl_easy_init());
}

curl::multi_ptr multi_init()
{
	return curl::multi_ptr(curl_multi_init());
}

const std::error_category& get_curl_category()
{
	static curl_category_impl s_category;
	return s_category;
}

void throw_if_failed(CURLcode code, std::string_view where)
{
	if (code != CURLcode::CURLE_OK)
	{
		throw std::system_error(code, get_curl_category(), where.data());
	}
}

curl_initializer::curl_initializer()
{
	CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
	throw_if_failed(code, "curl_global_init");
}

curl_initializer::~curl_initializer()
{
	curl_global_cleanup();
}

header_list::~header_list()
{
	curl_slist_free_all(m_head);
}

void header_list::add(std::wstring_view header)
{
	m_head = curl_slist_append(m_head, http::CurlUtils::wstring_to_utf8(header).data());
}

const curl_slist* header_list::get() const
{
	return m_head;
}

} // namespace curl
