#pragma once

#include "includes/cef_app.h"
#include "../JsonSpiritConverter.h"

namespace apputils
{

namespace detail
{

template <class T>
inline CefRefPtr<CefValue> ConvertReturnValue(const T &value)
{
	static_assert(!std::is_same<T, T>, "This function is not implemented for type T, please change return value type");
	return CefRefPtr<CefValue>(); // Prevent static analyzer warning on missing return value
}

template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const bool &value)
{
	auto pRet = CefValue::Create();
	pRet->SetBool(value);
	return pRet;
}

template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const int &value)
{
	auto pRet = CefValue::Create();
	pRet->SetInt(value);
	return pRet;
}


template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const float &value)
{
	auto pRet = CefValue::Create();
	pRet->SetDouble(double(value));
	return pRet;
}

template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const double &value)
{
	auto pRet = CefValue::Create();
	pRet->SetDouble(value);
	return pRet;
}

template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const std::string &value)
{
	auto pRet = CefValue::Create();
	pRet->SetString(value);
	return pRet;
}

template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const std::wstring &value)
{
	auto pRet = CefValue::Create();
	pRet->SetString(value);
	return pRet;
}

template<>
inline CefRefPtr<CefValue> ConvertReturnValue<>(const CefRefPtr<CefValue> &pValue)
{
	return pValue;
}

}

}
