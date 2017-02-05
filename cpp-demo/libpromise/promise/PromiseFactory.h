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
    explicit PromiseFactory(IDispatcher &callDispatcher, IDispatcher &callbackDispatcher)
        : m_callDispatcher(callDispatcher)
        , m_callbackDispatcher(callbackDispatcher)
    {
    }

    /// @brief Создаёт promise для функции, выполняемой в фоновом потоке
    /// Принимает функцию без аргументов, возвращающую результат операции
    /// Возврат либо исключение функции используется для заполнения Promise.
    /// Вызов callback'ов Promise выполняется в потоке callback'ов.
    /// Возвращает Promise от операции, запущенной через диспетчер задач фабрики.
    template <class TFunction>
    IPromisePtr<std::result_of_t<TFunction()> > MakePromise(TFunction && fn)
    {
        using ValueType = std::result_of_t<TFunction()>;
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
            if (result.which() == ISPROM_WHICH(ResultType, ValueType))
            {
                promise->Resolve(boost::get<ValueType>(result));
            }
            else
            {
                promise->Reject(boost::get<std::exception_ptr>(result));
            }
        });

        return IPromisePtr<ValueType>(promise);
    }

private:
    IDispatcher &m_callDispatcher;
    IDispatcher &m_callbackDispatcher;
};
}
