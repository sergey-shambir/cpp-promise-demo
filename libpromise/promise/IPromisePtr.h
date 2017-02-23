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

    // ������ ����������� (continuation) ����������� ��������
    // � ��������� ������������ ��� ������ � ��������� ������,
    //  ������� ��������� ���������� ��������� � ����� ������� ��� ��������.
    template<class Function, class Function2>
    decltype(auto) ThenDo(IDispatcher &callDispatcher, IDispatcher &callbackDispatcher, Function &&onFulfilled, Function2 &&onRejected)
    {
        using ResultType = std::result_of_t<Function(ValueType)>;
        using ResultType2 = std::result_of_t<Function2(const std::exception_ptr &)>;
        using ResultPromise = Promise<ResultType>;

        static_assert(std::is_same_v<ResultType, ResultType2>, "Then and Catch continuation functions should return the same type");

        // ���� ��������� ��� �� �����, ��������� ����������� ���������������.
        const bool immediateMode = (std::addressof(m_promise->GetDispatcher()) == std::addressof(callDispatcher));

        // ��������� ����� �������� � ������������ ������ �� ����������.
        auto submit = [immediateMode, &callDispatcher](Operation && operation) {
            if (immediateMode)
            {
                operation();
            }
            else
            {
                callDispatcher.Post(operation);
            }
        };

        auto promise = std::make_shared<ResultPromise>(callbackDispatcher);
        m_promise->Then([promise, onFulfilled, submit](ValueType value) {
            // TODO: add movable-only values support.
            //       possible solutions:
            //        1) make_shared<ValueType>(move(value)).
            submit([promise, onFulfilled, value] {
                try
                {
                    promise->Resolve(onFulfilled(value));
                }
                catch (...)
                {
                    promise->Reject(std::current_exception());
                }
            });
        });
        m_promise->Catch([submit, promise, onRejected](const std::exception_ptr &exception) {
            submit([promise, onRejected, exception] {
                try
                {
                    promise->Resolve(onRejected(exception));
                }
                catch (...)
                {
                    promise->Reject(std::current_exception());
                }
            });
        });

        return IPromisePtr<ResultType>(promise);
    }

    // ������ ����������� (continuation) ����������� ��������
    // � ��������� ������������ ��� ������ � ��������� ������,
    //  ������� ��������� ���������� ��������� � ����� ������� ��� ��������.
    template<class Function>
    decltype(auto) ThenDo(IDispatcher &callDispatcher, IDispatcher &callbackDispatcher, Function &&onFulfilled)
    {
        using ResultType = std::result_of_t<Function(ValueType)>;
        auto rethrowFn = [](const std::exception_ptr &exception) {
            std::rethrow_exception(exception);
            return ResultType();
        };
        return ThenDo(callDispatcher, callbackDispatcher, onFulfilled, rethrowFn);
    }

    // ������ ����������� (continuation) ����������� ��������
    // � ��������� ������������ ��� ������,
    //  ������� ��������� ���������� ��������� � ����� ������� ��� ��������.
    template<class Function, class Function2>
    decltype(auto) ThenDo(IDispatcher &callDispatcher, Function && onFulfilled, Function2 &&onRejected)
    {
        return ThenDo(callDispatcher, m_promise->GetDispatcher(), std::forward<Function>(onFulfilled), std::forward<Function2>(onRejected));
    }

    // ������ ����������� (continuation) ����������� ��������
    // � ��������� ������������ ��� ������,
    //  ������� ��������� ���������� ��������� � ����� ������� ��� ��������.
    template<class Function>
    decltype(auto) ThenDo(IDispatcher &callDispatcher, Function && onFulfilled)
    {
        return ThenDo(callDispatcher, m_promise->GetDispatcher(), std::forward<Function>(onFulfilled));
    }

    // ������ ����������� (continuation) ����������� ��������,
    //  ������� ��������� ���������� ��������� � ����� ������� ��� ��������.
    template<class Function, class Function2>
    decltype(auto) ThenDo(Function && onFulfilled, Function2 &&onRejected)
    {
        return ThenDo(m_promise->GetDispatcher(), m_promise->GetDispatcher(), std::forward<Function>(onFulfilled), std::forward<Function2>(onRejected));
    }

    // ������ ����������� (continuation) ����������� ��������,
    //  ������� ��������� ���������� ��������� � ����� ������� ��� ��������.
    template<class Function>
    decltype(auto) ThenDo(Function && onFulfilled)
    {
        return ThenDo(m_promise->GetDispatcher(), m_promise->GetDispatcher(), std::forward<Function>(onFulfilled));
    }

private:
    std::shared_ptr<IPromiseType> m_promise;
};
}
