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

	// Создаёт продолжение (continuation) асинхронной операции
	// с указанным исполнителем для вызова и обратного вызова,
	//  передаёт результат предыдущей подзадачи в новую функцию как параметр.
    template<class Function>
    IPromisePtr ThenDo(IDispatcher &callDispatcher, IDispatcher &callbackDispatcher, Function && fn)
	{
		using ResultType = decltype(Function(ValueType));
		using ResultPromise = Promise<ResultType>;

		// Если диспетчер тот же самый, выполняем продолжение безотлагательно.
		const bool immediateMode = (std::addressof(m_promise->GetDispatcher()) == std::addressof(callDispatcher));

		auto promise = std::make_shared<ResultPromise>(callbackDispatcher);
		m_promise->Then([immediateMode, promise, fn](ValueType value) {
			auto task = [promise, fn] {
				try
				{
					auto value = fn(std::move(value));
					promise->Resolve(std::move(value));
				}
				catch (...)
				{
					promise->Reject(std::current_exception());
				}
			};
			if (immediateMode)
			{
				task();
			}
			else
			{
				callDispatcher.Post(task);
			}
		});
		m_promise->Catch([immediateMode, promise](const std::exception &exception) {
			auto task = std::bind(ResultPromise::Reject, promise, exception);
			if (immediateMode)
			{
				task();
			}
			else
			{
				callDispatcher.Post(task);
			}
		});

		return promise;
    }

	// Создаёт продолжение (continuation) асинхронной операции
	// с указанным исполнителем для вызова,
	//  передаёт результат предыдущей подзадачи в новую функцию как параметр.
	template<class Function>
	IPromisePtr ThenDo(IDispatcher &callDispatcher, Function && fn)
	{
		return ThenDo(callDispatcher, m_promise->GetDispatcher(), std::forward<Function>(fn));
	}

	// Создаёт продолжение (continuation) асинхронной операции,
	//  передаёт результат предыдущей подзадачи в новую функцию как параметр.
	template<class Function>
	IPromisePtr ThenDo(Function && fn)
	{
		return ThenDo(m_promise->GetDispatcher(), m_promise->GetDispatcher(), std::forward<Function>(fn));
	}

private:
    std::shared_ptr<IPromiseType> m_promise;
};
}
