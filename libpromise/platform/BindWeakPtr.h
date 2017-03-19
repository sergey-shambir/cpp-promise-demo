#include <functional>
#include <iostream>
#include <memory>

/// Function `BindWeakPtr` - safe `std::bind` replacement for `std::weak_ptr`
///  Implements "weak this" idiom: callback called only if owner object still exists, otherwise nothing happens.
/// See example below:
///        auto pSharedBed = std::make_shared<CTestbed>();
///        auto fn = BindWeakPtr(&CTestbed::Print, pSharedBed, 42);
///        fn(); // will be called
///        pSharedBed = nullptr;
///        fn(); // will not be called

namespace detail
{
template<class ReturnType, class ClassType, bool AddConst, class... Args>
struct WeakInvoker
{
    using ConstMethodType = ReturnType (ClassType::*)(Args... args) const;
    using NonConstMethodType = ReturnType (ClassType::*)(Args... args);
    using MethodType = std::conditional_t<AddConst, ConstMethodType, NonConstMethodType>;
    using WeakPtrType = std::weak_ptr<ClassType>;

    WeakInvoker(MethodType pMethod, WeakPtrType &&pObject)
        : m_pMethod(pMethod)
        , m_pObject(pObject)
    {
    }

    ReturnType operator()(Args... args) const
    {
        if (auto pThis = m_pObject.lock())
        {
            return (pThis.get()->*m_pMethod)(std::forward<Args>(args)...);
        }
        return ReturnType();
    }

    MethodType m_pMethod;
    WeakPtrType m_pObject;
};
}

/// Weak this binding of non-const methods.
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType (ClassType::*memberFn)(Params... args), std::shared_ptr<ClassType> const &pThis, Args... args)
{
    using Invoker = detail::WeakInvoker<ReturnType, ClassType, false, Params...>;

    Invoker invoker(memberFn, std::weak_ptr<ClassType>(pThis));
    return std::bind(invoker, args...);
}

/// Weak this binding of const methods.
template<typename ReturnType, typename ClassType, typename... Params, typename... Args>
decltype(auto) BindWeakPtr(ReturnType (ClassType::*memberFn)(Params... args) const, std::shared_ptr<ClassType> const &pThis, Args... args)
{
    using Invoker = detail::WeakInvoker<ReturnType, ClassType, true, Params...>;

    Invoker invoker(memberFn, std::weak_ptr<ClassType>(pThis));
    return std::bind(invoker, args...);
}
