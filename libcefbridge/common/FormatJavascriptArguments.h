#pragma once

#include "is_any_of.h"
#include "ispringutils/datetime/Timestamp.h"
#include <json_spirit_value.h>

namespace apputils
{
namespace detail
{

class JsFormatter
{
public:
	template <class T>
	using CanFormatType = is_any_of<T, bool, double, std::string, std::wstring, ispringutils::datetime::CTimestamp,
		json_spirit::Array, json_spirit::wArray, json_spirit::Value, json_spirit::wValue, json_spirit::Object, json_spirit::wObject>;

	void Begin();
	std::string Finalize();

	template <class T>
	void operator ()(T && value)
	{
		static_assert(CanFormatType<std::decay_t<T>>{}, "Cannot map argument type to Javascript, change type or cast it explicitly");
		Add(value);
	}

private:
	void Add(bool value);
	void Add(double value);
	void Add(const std::string &value);
	void Add(const std::wstring &value);
	void Add(ispringutils::datetime::CTimestamp timestamp);
	void Add(const json_spirit::wArray &value);
	void Add(const json_spirit::Array &value);
	void Add(const json_spirit::wObject &value);
	void Add(const json_spirit::Object &value);
	void Add(const json_spirit::wValue &value);
	void Add(const json_spirit::Value &value);
	void BeforeAddArgument();

	bool m_addComma = false;
	std::string m_code;
};

inline void ApplyFormatter(JsFormatter &)
{
}

template <class T, class ...TArgs>
inline void ApplyFormatter(JsFormatter &formatter, T && value, TArgs&&... args)
{
	formatter(std::forward<T>(value));
	ApplyFormatter(formatter, std::forward<TArgs>(args)...);
}

template <class ...TArgs>
std::string FormatJavascriptArguments(TArgs&&... args)
{
	JsFormatter formatter;
	formatter.Begin();
	ApplyFormatter(formatter, std::forward<TArgs>(args)...);

	return formatter.Finalize();
}

}

}
