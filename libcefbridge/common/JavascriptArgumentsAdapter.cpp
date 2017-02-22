#include "../stdafx.h"
#include "JavascriptArgumentsAdapter.h"

namespace
{
using nlohmann::json;

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

class JsonConverter
{
public:
    json operator()(const CefRefPtr<CefValue> &value)
    {
        switch (value->GetType())
        {
        case VTYPE_DICTIONARY:
            return operator ()(value->GetDictionary());
        case VTYPE_LIST:
            return operator ()(value->GetList());
        case VTYPE_STRING:
            return operator ()(value->GetString());
        case VTYPE_BOOL:
            return operator ()(value->GetBool());
        case VTYPE_DOUBLE:
            return operator ()(value->GetDouble());
        case VTYPE_INT:
            return operator ()(value->GetInt());
        case VTYPE_BINARY:
            throw std::runtime_error("Cannot convert CEF binary value to JSON");
        default:
            throw std::runtime_error("Unexpected CEF value type");
        }
    }

    json operator()(const CefRefPtr<CefDictionaryValue> &dict)
    {
        CefDictionaryValue::KeyList keys;
        dict->GetKeys(keys);
        json obj = json::object();
        for (const CefString &key : keys)
        {
            obj[key.ToString()] = operator ()(dict->GetValue(key));
        }

        return obj;
    }

    json operator()(const CefRefPtr<CefListValue> &list)
    {
        json arr = json::array();
        for (int i = 0, count = int(list->GetSize()); i < count; ++i)
        {
            arr.push_back(operator()(list->GetValue(i)));
        }

        return arr;
    }

    json operator()(const CefString &value)
    {
        return json(value.ToString());
    }

    json operator()(bool value)
    {
        return json(value);
    }

    json operator()(double value)
    {
        return json(value);
    }

    json operator()(int value)
    {
        return json(value);
    }
};
}

namespace cefbridge
{
namespace detail
{

JavascriptArgumentsAdapter::JavascriptArgumentsAdapter(const CefRefPtr<CefListValue> &arguments)
	: m_arguments(arguments)
{
}

void JavascriptArgumentsAdapter::CheckArgumentsCount(size_t expectedCount)const
{
	if (expectedCount != m_arguments->GetSize())
	{
		const std::string expectedStr = std::to_string(expectedCount);
		const std::string gotStr = std::to_string(m_arguments->GetSize());
		throw std::invalid_argument("Argument count mismatch: " + expectedStr + " expected, got " + gotStr);
	}
}

void JavascriptArgumentsAdapter::Convert(bool & destination, size_t index) const
{
	assert(index < static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_BOOL);
	destination = m_arguments->GetBool(static_cast<int>(index));
}

void JavascriptArgumentsAdapter::Convert(int & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_INT);
	destination = m_arguments->GetInt(static_cast<int>(index));
}

void JavascriptArgumentsAdapter::Convert(double & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_DOUBLE);
	destination = m_arguments->GetDouble(static_cast<int>(index));
}

void JavascriptArgumentsAdapter::Convert(std::string & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_STRING);
	destination = m_arguments->GetString(static_cast<int>(index)).ToString();
}

void JavascriptArgumentsAdapter::Convert(std::wstring & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_STRING);
	destination = m_arguments->GetString(static_cast<int>(index)).ToWString();
}

void JavascriptArgumentsAdapter::Convert(CefRefPtr<CefListValue> & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_LIST);
	destination = m_arguments->GetList(static_cast<int>(index));
}

void JavascriptArgumentsAdapter::Convert(CefRefPtr<CefDictionaryValue> &destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_DICTIONARY);
	destination = m_arguments->GetDictionary(static_cast<int>(index));
}

void JavascriptArgumentsAdapter::Convert(CefRefPtr<CefBinaryValue> & destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_BINARY);
	destination = m_arguments->GetBinary(static_cast<int>(index));
}

void JavascriptArgumentsAdapter::Convert(nlohmann::json &destination, size_t index) const
{
	assert(index <= static_cast<int>(INT_MAX));
	CheckArgument(index, VTYPE_LIST);
	auto pListValue = m_arguments->GetList(static_cast<int>(index));

    JsonConverter converter;
    destination = converter(pListValue);
}

void JavascriptArgumentsAdapter::CheckArgument(size_t index, CefValueType type) const
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
