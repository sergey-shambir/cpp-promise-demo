#pragma once
#include <functional>

namespace isprom
{
using Operation = std::function<void()>;
using CancelPredicate = std::function<bool()>;
using CancelableOperation = std::function<void(CancelPredicate const &didCancel)>;
}
