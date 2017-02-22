#pragma once
#include "Promise.h"
#include "IPromisePtr.h"
#include "promise_detail.h"
#include <type_traits>
#include <boost/optional.hpp>

namespace isprom
{
class PromiseFactory
{
public:
    /// @param callDispatcher - диспетчер для запуска вызовов
    /// @param callbackDispatcher - диспетчер для запуска колбеков
    explicit PromiseFactory(IDispatcher &callDispatcher, IDispatcher &callbackDispatcher);

    /// @brief Создаёт promise для функции, выполняемой в фоновом потоке
    /// Принимает функцию без аргументов, возвращающую результат операции
    /// Возврат либо исключение функции используется для заполнения Promise.
    /// Вызов callback'ов Promise выполняется в потоке callback'ов.
    /// Возвращает Promise от операции, запущенной через диспетчер задач фабрики.
    template <class Function>
    IPromisePtr<std::result_of_t<Function()>> MakePromise(Function && fn)
    {
        using ValueType = std::result_of_t<Function()>;
        using ResultType = boost::variant<std::exception_ptr, ValueType>;

        auto promise = std::make_shared<Promise<ValueType>>(m_callbackDispatcher);
        m_callDispatcher.Post([fn, promise] {
            ResultType result;
            try
            {
                auto value = fn();
                static_assert(std::is_same<decltype(value), ValueType>::value, "unexpected internal types mismatch");
                result = std::move(value);
            }
            catch (...)
            {
                result = std::current_exception();
            }
            if (result.which() == detail::WhichIndex<ResultType, ValueType>::value)
            {
                promise->Resolve(std::move_if_noexcept(boost::get<ValueType>(result)));
            }
            else
            {
                promise->Reject(std::move_if_noexcept(boost::get<std::exception_ptr>(result)));
            }
        });

        return IPromisePtr<ValueType>(promise);
    }

private:
    IDispatcher &m_callDispatcher;
    IDispatcher &m_callbackDispatcher;
};
}
