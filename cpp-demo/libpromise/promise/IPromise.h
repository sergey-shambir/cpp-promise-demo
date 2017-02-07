#pragma once
#include "../Types.h"
#include <functional>
#include <stdexcept>

namespace isprom
{
template <class ValueType>
class IPromise
{
public:
    using ThenFunction = std::function<void(ValueType)>;
    using CatchFunction = std::function<void(std::exception_ptr const&)>;

    virtual ~IPromise() = default;

    virtual void Then(const ThenFunction &onFulfilled) = 0;
    virtual void Catch(const CatchFunction &onRejected) = 0;
    virtual void Cancel() = 0;
};
}
