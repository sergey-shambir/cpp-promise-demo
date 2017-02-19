#pragma once

#include "includes/cef_app.h"
#include "ispringutils/datetime/Timestamp.h"
#include "is_any_of.h"
#include <json_spirit_value.h>

namespace apputils
{
namespace detail
{

class CJavascriptArgumentsAdapter
{
public:
	template <class T>
	using CanConvertType = is_any_of<T, bool, double, std::string, std::wstring,
		ispringutils::datetime::CTimestamp, json_spirit::wArray, json_spirit::wObject,
		CefRefPtr<CefListValue>, CefRefPtr<CefDictionaryValue>, CefRefPtr<CefBinaryValue>>;

	CJavascriptArgumentsAdapter(const CefRefPtr<CefListValue> &arguments);
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
	void Convert(json_spirit::wArray &destination, size_t index)const;
	void Convert(json_spirit::wObject &destination, size_t index)const;
	void Convert(CefRefPtr<CefListValue> &destination, size_t index)const;
	void Convert(CefRefPtr<CefDictionaryValue> &destination, size_t index)const;
	void Convert(CefRefPtr<CefBinaryValue> &destination, size_t index)const;
	void Convert(ispringutils::datetime::CTimestamp &destination, size_t index)const;
	void CheckArgument(size_t index, CefValueType type)const;

	CefRefPtr<CefListValue> m_arguments;
};

}
}
