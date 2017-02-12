#pragma once
#include "IPromise.h"
#include "Promise.h"
#include <memory>
#include <type_traits>

namespace isprom
{
template <class Value>
class IPromisePtr
{
public:
    using ValueType = Value;
    using IPromiseType = IPromise<Value>;
    using ThenFunction = typename IPromiseType::ThenFunction;
    using CatchFunction = typename IPromiseType::CatchFunction;

    IPromisePtr() = default;

    explicit IPromisePtr(const std::shared_ptr<IPromiseType> &promise)
        : m_promise(promise)
    {
    }

    void Then(const ThenFunction &onFulfilled)const
    {
        m_promise->Then(onFulfilled);
    }

    void Then(const ThenFunction &onFulfilled, const CatchFunction &onRejected)const
    {
        m_promise->Then(onFulfilled);
        m_promise->Catch(onFulfilled);
    }

    void Catch(const CatchFunction &onRejected)const
    {
        m_promise->Catch(onRejected);
    }

    void Cancel()const
    {
        m_promise->Cancel();
    }

#if 0
    template<class Function>
    IPromisePtr Continue(Function && fn)
    {
        using ResultValue = std::result_of_t<Function(ValueType)>;

#error TODO
    }
#endif

private:
    std::shared_ptr<IPromiseType> m_promise;
};
}
