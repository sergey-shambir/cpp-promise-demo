#pragma once

#include "include/cef_app.h"

template <class T, class ...TArgs>
CefRefPtr<T> MakeCefRef(TArgs&&... args)
{
    return CefRefPtr<T>(new T(std::forward<TArgs>(args)...));
}
