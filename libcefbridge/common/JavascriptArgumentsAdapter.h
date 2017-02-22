#pragma once

#include "include/cef_app.h"
#include "is_any_of.h"
#include <json.hpp>

namespace cefbridge
{
namespace detail
{

class JavascriptArgumentsAdapter
{
public:
	template <class T>
	using CanConvertType = is_any_of<T, bool, double, std::string, std::wstring,
        nlohmann::json, CefRefPtr<CefListValue>, CefRefPtr<CefDictionaryValue>, CefRefPtr<CefBinaryValue>>;

    JavascriptArgumentsAdapter(const CefRefPtr<CefListValue> &arguments);
	void CheckArgumentsCount(size_t expectedCount)const;

	template<class T>
	void operator()(T & destination, size_t index)const
	{
		static_assert(CanConvertType<std::decay_t<T>>{}, "argument conversion is not implemented for type T, please use another type");
		Convert(destination, index);
	}

private:
	void Convert(bool &destination, size_t index)const;
	void Convert(int &destination, size_t index)const;
	void Convert(double &destination, size_t index)const;
	void Convert(std::string &destination, size_t index)const;
	void Convert(std::wstring &destination, size_t index)const;
    void Convert(nlohmann::json &destination, size_t index)const;
	void Convert(CefRefPtr<CefListValue> &destination, size_t index)const;
	void Convert(CefRefPtr<CefDictionaryValue> &destination, size_t index)const;
    void Convert(CefRefPtr<CefBinaryValue> &destination, size_t index)const;
	void CheckArgument(size_t index, CefValueType type)const;

	CefRefPtr<CefListValue> m_arguments;
};

}
}
