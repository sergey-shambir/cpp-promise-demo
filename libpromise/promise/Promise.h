#pragma once
#include "PromiseObject.h"
#include <memory>
#include <type_traits>

namespace isc
{
template<class ValueType>
class Promise
{
public:
	using Value = ValueType;
	using ValuePromiseObject = PromiseObject<Value>;
	using ThenFunction = typename ValuePromiseObject::ThenFunction;
	using CatchFunction = typename ValuePromiseObject::CatchFunction;
	using CancelFunction = typename ValuePromiseObject::CancelFunction;
	using DispatcherWeakPtr = typename ValuePromiseObject::DispatcherWeakPtr;

	Promise() = default;

	explicit Promise(const std::shared_ptr<ValuePromiseObject>& promise)
		: m_promise(promise)
	{
	}

	explicit operator bool() const
	{
		return bool(m_promise);
	}

	const Promise& Then(const ThenFunction& onFulfilled) const
	{
		assert(m_promise);
		m_promise->Then(onFulfilled);
		return *this;
	}

	Promise& Then(const ThenFunction& onFulfilled)
	{
		assert(m_promise);
		m_promise->Then(onFulfilled);
		return *this;
	}

	const Promise& Catch(const CatchFunction& onRejected) const
	{
		assert(m_promise);
		m_promise->Catch(onRejected);
		return *this;
	}

	Promise& Catch(const CatchFunction& onRejected)
	{
		assert(m_promise);
		m_promise->Catch(onRejected);
		return *this;
	}

	void Cancel()
	{
		assert(m_promise);
		m_promise->Cancel();
		m_promise.reset();
	}

	/// @param onFulfilled should return Promise<T>
	template<class AsyncFunction>
	decltype(auto) ThenDoAsync(AsyncFunction&& onFulfilled) const
	{
		assert(m_promise);

		using ResultPromise = std::result_of_t<AsyncFunction(Value)>;
		using Result = typename ResultPromise::Value;
		using ResultPromiseObject = PromiseObject<Result>;

		static_assert(std::is_same<ResultPromise, Promise<Result>>::value,
			"async function for continuation must return Promise<T>");

		auto promise = std::make_shared<ResultPromiseObject>(m_promise->GetDispatcher());
		m_promise->Then([promise, onFulfilled](auto&& value) {
			try
			{
				auto resultPromise = onFulfilled(std::move(value));
				resultPromise.Then([promise](auto&& value) {
					promise->Resolve(std::move(value));
				});
				resultPromise.Catch([promise](const std::exception_ptr& exception) {
					promise->Reject(exception);
				});
			}
			catch (...)
			{
				promise->Reject(std::current_exception());
			}
		});
		m_promise->Catch([promise](const std::exception_ptr& exception) {
			promise->Reject(exception);
		});

		std::weak_ptr<ValuePromiseObject> weakPromise = m_promise;
		promise->DoOnCancel([weakPromise] {
			if (auto nestedPromise = weakPromise.lock())
			{
				nestedPromise->Cancel();
			}
		});

		return ResultPromise(promise);
	}

// Disable C4702 unreachable code: Resolve() is unreachable if onRejected always throws.
#if defined(_MSC_VER)
#pragma warning(push, 4)
#pragma warning(disable : 4702)
#endif

	/// Creates continuation of async operation
	///  with given dispatchers for call and callback,
	///  passes first task result to onFulfilled, or exception to onRejected
	template<class Function, class Function2>
	decltype(auto) ThenDo(DispatcherWeakPtr callDispatcher, DispatcherWeakPtr callbackDispatcher, Function&& onFulfilled, Function2&& onRejected) const
	{
		assert(m_promise);
		// TODO: <sergey.shambir> call cancel for nested promise when new promise canceled.

		using ResultType = std::result_of_t<Function(Value)>;
		using ResultType2 = std::result_of_t<Function2(const std::exception_ptr&)>;
		using ResultPromise = PromiseObject<ResultType>;

		static_assert(std::is_same_v<ResultType, ResultType2>, "Then and Catch continuation functions should return the same type");

		// Execute continuation immediately for the same dispatcher.
		auto firstDispatcher = m_promise->GetDispatcher().lock();
		auto secondDispatcher = callDispatcher.lock();
		const bool immediateMode = (bool(firstDispatcher) && (firstDispatcher.get() == secondDispatcher.get()));

		// Dispatches new operation with nested continuation.
		auto submit = [immediateMode, callDispatcher](auto&& operation) {
			if (immediateMode)
			{
				operation();
			}
			else if (auto dispatcher = callDispatcher.lock())
			{
				dispatcher->Dispatch(operation);
			}
		};

		auto promise = std::make_shared<ResultPromise>(callbackDispatcher);
		m_promise->Then([promise, onFulfilled, submit](Value value) {
			// TODO: <sergey.shambir> add movable-only values support.
			//       possible solutions:
			//        1) make_shared<Value>(move(value)).
			submit([promise, onFulfilled, value] {
				try
				{
					promise->Resolve(onFulfilled(Value(std::move_if_noexcept(value))));
				}
				catch (...)
				{
					promise->Reject(std::current_exception());
				}
			});
		});
		m_promise->Catch([submit, promise, onRejected](const std::exception_ptr& exception) {
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

		std::weak_ptr<ValuePromiseObject> weakPromise = m_promise;
		promise->DoOnCancel([weakPromise] {
			if (auto promise = weakPromise.lock())
			{
				promise->Cancel();
			}
		});

		return Promise<ResultType>(promise);
	}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	/// Creates continuation of async operation
	///  with given dispatchers for call and callback,
	///  passes first task result to onFulfilled.
	template<class Function>
	decltype(auto) ThenDo(DispatcherWeakPtr callDispatcher, DispatcherWeakPtr callbackDispatcher, Function&& onFulfilled) const
	{
		assert(m_promise);
		using ResultType = std::result_of_t<Function(Value)>;
		auto rethrowFn = [](const std::exception_ptr& exception) -> ResultType {
			std::rethrow_exception(exception);
		};
		return ThenDo(callDispatcher, callbackDispatcher, onFulfilled, rethrowFn);
	}

	/// Creates continuation of async operation
	///  with promise dispatcher for call and callback,
	///  passes first task result to onFulfilled.
	template<class Function, class Function2>
	decltype(auto) ThenDo(Function&& onFulfilled, Function2&& onRejected) const
	{
		assert(m_promise);
		return ThenDo(m_promise->GetDispatcher(), m_promise->GetDispatcher(), std::forward<Function>(onFulfilled), std::forward<Function2>(onRejected));
	}

	/// Creates continuation of async operation
	///  with promise dispatcher for call and callback,
	///  passes first task result to onFulfilled, or exception to onRejected
	template<class Function>
	decltype(auto) ThenDo(Function&& onFulfilled) const
	{
		assert(m_promise);
		return ThenDo(m_promise->GetDispatcher(), m_promise->GetDispatcher(), std::forward<Function>(onFulfilled));
	}

private:
	std::shared_ptr<ValuePromiseObject> m_promise;
};

} // namespace isc
