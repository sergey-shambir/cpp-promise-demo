#include "stdafx.h"
#include "JavascriptArgumentsAdapter.h"
#include "libcef_client/common/BinaryValueUtils.h"
#include "../JsonSpiritConverter.h"

namespace
{
std::string GetCefValueTypename(CefValueType type)
{
	switch (type)
	{
	case VTYPE_INVALID:
		break;
	case VTYPE_NULL:
		return "null or undefined";
	case VTYPE_BOOL:
		return "Boolean";
	case VTYPE_INT:
		return "int";
	case VTYPE_DOUBLE:
		return "Number";
	case VTYPE_STRING:
		return "String";
	case VTYPE_BINARY:
		return "encoded binary";
	case VTYPE_DICTIONARY:
		return "Object";
	case VTYPE_LIST:
		return "Array";
	}
	assert(false);
	return "Invalid Value";
}
}

namespace apputils
{
namespace detail
{

CJavascriptArgumentsAdapter::CJavascriptArgumentsAdapter(const CefRefPtr<CefListValue> &arguments)
	: m_arguments(arguments)
{
}

void CJavascriptArgumentsAdapter::CheckArgumentsCount(size_t expectedCount)const
{
	if (expectedCount != m_arguments->GetSize())
	{
		const std::string expectedStr = std::to_string(expectedCount);
		const std::string gotStr = std::to_string(m_arguments->GetSize());
		throw std::invalid_argument("Argument count mismatch: " + expectedStr + " expected, got " + gotStr);
	}
}

void CJavascriptArgumentsAdapter::Convert(bool & destination, size_t index) const
{
	assert(index < static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_BOOL);
	destination = m_arguments->GetBool(static_cast<int>(index));
}

void CJavascriptArgumentsAdapter::Convert(int & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_INT);
	destination = m_arguments->GetInt(static_cast<int>(index));
}

void CJavascriptArgumentsAdapter::Convert(double & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_DOUBLE);
	destination = m_arguments->GetDouble(static_cast<int>(index));
}

void CJavascriptArgumentsAdapter::Convert(std::string & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_STRING);
	destination = m_arguments->GetString(static_cast<int>(index)).ToString();
}

void CJavascriptArgumentsAdapter::Convert(std::wstring & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_STRING);
	destination = m_arguments->GetString(static_cast<int>(index)).ToWString();
}

void CJavascriptArgumentsAdapter::Convert(CefRefPtr<CefListValue> & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_LIST);
	destination = m_arguments->GetList(static_cast<int>(index));
}

void CJavascriptArgumentsAdapter::Convert(CefRefPtr<CefDictionaryValue> &destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_DICTIONARY);
	destination = m_arguments->GetDictionary(static_cast<int>(index));
}

void CJavascriptArgumentsAdapter::Convert(CefRefPtr<CefBinaryValue> & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_BINARY);
	destination = m_arguments->GetBinary(static_cast<int>(index));
}

void CJavascriptArgumentsAdapter::Convert(ispringutils::datetime::CTimestamp & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_BINARY);
	auto pBinaryValue = m_arguments->GetBinary(static_cast<int>(index));

	CefTime datetime;
	if (!cefclient::CBinaryValueUtils::ReadDatetime(pBinaryValue, datetime))
	{
		const std::string indexStr = std::to_string(index);
		throw std::invalid_argument("Argument #" + indexStr + " is not valid serialized Date object");
	}

	std::tm stdDatetime = { 0 };
	stdDatetime.tm_year = datetime.year - 1900;
	stdDatetime.tm_mon = datetime.month - 1;
	stdDatetime.tm_mday = datetime.day_of_month;
	stdDatetime.tm_hour = datetime.hour;
	stdDatetime.tm_min = datetime.minute;
	stdDatetime.tm_sec = datetime.second;
	destination = ispringutils::datetime::CTimestamp(stdDatetime);
}

void CJavascriptArgumentsAdapter::Convert(json_spirit::wArray &destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_LIST);
	auto pListValue = m_arguments->GetList(static_cast<int>(index));

	CJsonSpiritConverter converter;
	destination = converter.ConvertCefList(pListValue);
}

void CJavascriptArgumentsAdapter::Convert(json_spirit::wObject &destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_DICTIONARY);
	auto pDictValue = m_arguments->GetDictionary(static_cast<int>(index));

	CJsonSpiritConverter converter;
	destination = converter.ConvertCefDict(pDictValue);
}

void CJavascriptArgumentsAdapter::CheckArgument(size_t index, CefValueType type) const
{
	assert(index <= static_cast<int>(INT_MAX));
	if (index >= m_arguments->GetSize())
	{
		const std::string indexStr = std::to_string(index);
		const std::string sizeStr = std::to_string(m_arguments->GetSize());
		throw std::invalid_argument("Argument #" + indexStr + " does not exist: only " + sizeStr + " arguments passed");
	}
	if (m_arguments->GetType(static_cast<int>(index)) != type)
	{
		const std::string typeExpected = GetCefValueTypename(type);
		const std::string typeGot = GetCefValueTypename(m_arguments->GetType(static_cast<int>(index)));
		const std::string indexStr = std::to_string(index);
		throw std::invalid_argument("Argument #" + indexStr + " must have type " + typeExpected + ", type " + typeGot + " passed");
	}
}

}
}
