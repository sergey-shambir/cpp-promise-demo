#include "stdafx.h"
#include "FormatJavascriptArguments.h"
#include "cpsutils/include/actionscript_utils.h"
#include "cpsutils/include/string_utils.h"
#include "includes/min_max_fix.hpp"
#include "libcef_client/common/Errors.h"
#include <json_spirit_writer_template.h>
#include <json_spirit_writer_options.h>
#include <cctype>


using cpsutils::actionscript_utils;
using cpsutils::string_utils;
using std::string;
using std::wstring;
using wValue = json_spirit::wValue;
using Value = json_spirit::Value;


namespace apputils
{
namespace detail
{
namespace
{
const unsigned JSON_FLAGS = unsigned(json_spirit::remove_trailing_zeros);

string WriteJsonString(const wValue &value)
{
	return string_utils::wstring_to_utf8(json_spirit::write_string(value, JSON_FLAGS));
}

string WriteJsonString(const Value &value)
{
	return json_spirit::write_string(value, JSON_FLAGS);
}
}

void JsFormatter::Begin()
{
	m_code += "[";
	m_addComma = false;
}

std::string JsFormatter::Finalize()
{
	m_code += "]";
	return std::move(m_code);
}

void JsFormatter::Add(bool value)
{
	BeforeAddArgument();
	m_code += (value ? "true" : "false");
}

void JsFormatter::Add(double value)
{
	if (!isfinite(value))
	{
		throw std::invalid_argument(cefclient::CEF_NONFINITE_NUMBER_ERROR);
	}

	BeforeAddArgument();
	m_code += std::to_string(value);
}

void JsFormatter::Add(const std::wstring &value)
{
	BeforeAddArgument();
	m_code += string_utils::quoted(string_utils::wstring_to_utf8(actionscript_utils::escape_action_script_string(value)));
}

void JsFormatter::Add(const std::string &value)
{
	BeforeAddArgument();
	m_code += string_utils::quoted(actionscript_utils::escape_action_script_string(value));
}

void JsFormatter::Add(const json_spirit::wArray &value)
{
	Add(wValue(value));
}

void JsFormatter::Add(const json_spirit::Array &value)
{
	Add(Value(value));
}

void JsFormatter::Add(const json_spirit::wObject &value)
{
	Add(wValue(value));
}

void JsFormatter::Add(const json_spirit::Object &value)
{
	Add(Value(value));
}

void JsFormatter::Add(const json_spirit::wValue &value)
{
	BeforeAddArgument();
	m_code += WriteJsonString(value);
}

void JsFormatter::Add(const json_spirit::Value &value)
{
	BeforeAddArgument();
	m_code += WriteJsonString(value);
}

// Adds `new Date(Date.UTC(year, month, day, hour, minute, second))`
void JsFormatter::Add(ispringutils::datetime::CTimestamp timestamp)
{
	BeforeAddArgument();

	const std::tm datetime = timestamp.GetTimeUTC();

	std::stringstream out;
	out << "new Date(Date.UTC("
		<< datetime.tm_year << ","
		<< datetime.tm_mon << ","
		<< datetime.tm_mday << ","
		<< datetime.tm_hour << ","
		<< datetime.tm_min << ","
		<< min(datetime.tm_sec, 59) // Does not allow 60th "leap" second.
		<< "))"; 

	m_code += out.str();
}

void JsFormatter::BeforeAddArgument()
{
	if (m_addComma)
	{
		m_code += ',';
	}
	m_addComma = true;
}

}
}

