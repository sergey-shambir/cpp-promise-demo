#include "../stdafx.h"
#include "FormatJavascriptArguments.h"
#include "Errors.h"
#include <cctype>
#include <codecvt>


namespace cefbridge
{
namespace detail
{
namespace
{
// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}

// convert wstring to UTF-8 string
std::string wstring_to_utf8(const std::wstring &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}

std::string escape_string(const std::string &str)
{
    return nlohmann::json(str).dump();
}

std::string escape_wstring(const std::wstring &str)
{
    return escape_string(wstring_to_utf8(str));
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
    if (!std::isfinite(value))
	{
        throw std::invalid_argument(errors::CEF_NONFINITE_NUMBER_ERROR);
	}

	BeforeAddArgument();
	m_code += std::to_string(value);
}

void JsFormatter::Add(const std::wstring &value)
{
	BeforeAddArgument();
    m_code += escape_wstring(value);
}

void JsFormatter::Add(const nlohmann::json &value)
{
    BeforeAddArgument();
    m_code += value.dump();
}

void JsFormatter::Add(const std::string &value)
{
	BeforeAddArgument();
    m_code += escape_string(value);
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

