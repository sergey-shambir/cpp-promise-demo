#pragma once
#include "../platform/IDispatcher.h"
#include <functional>
#include <mutex>
#include <stdexcept>
#include <variant>
#include <cassert>
#include <thread>

namespace isc
{
template<typename ValueType>
class PromiseObject
	: public std::enable_shared_from_this<PromiseObject<ValueType>>
{
public:
	using Value = ValueType;
	using ThenFunction = std::function<void(Value)>;
	using CatchFunction = std::function<void(std::exception_ptr const&)>;
	using CancelFunction = std::function<void()>;
	using DispatcherWeakPtr = std::weak_ptr<isc::IDispatcher>;

	PromiseObject(DispatcherWeakPtr dispatcher)
		: m_dispatcher(dispatcher)
	{
	}

	PromiseObject(const PromiseObject&) = delete;
	PromiseObject& operator=(const PromiseObject&) = delete;

	DispatcherWeakPtr GetDispatcher()
	{
		return m_dispatcher;
	}

	void Then(const ThenFunction& onFulfilled)
	{
		lock_guard lock(m_mutex);
		if (m_then)
		{
			throw std::logic_error("Cannot call Then twice (implementation limitation)");
		}

		switch (m_storage.index())
		{
		case PendingStateIndex:
			m_then = onFulfilled;
			break;
		case FulfilledStateIndex:
			m_then = onFulfilled;
			InvokeThen();
			break;
		default:
			break;
		}
	}

	void Catch(const CatchFunction& onRejected)
	{
		lock_guard lock(m_mutex);
		if (m_catch)
		{
			throw std::logic_error("Cannot call Catch twice (implementation limitation)");
		}

		switch (m_storage.index())
		{
		case PendingStateIndex:
			m_catch = onRejected;
			break;
		case RejectedStateIndex:
			m_catch = onRejected;
			InvokeCatch();
			break;
		default:
			break;
		}
	}

	void DoOnCancel(const CancelFunction& cancel)
	{
		lock_guard lock(m_mutex);
		if (m_cancel)
		{
			throw std::logic_error("Cannot call DoOnCancel twice (implementation limitation)");
		}

		// Assignment can throw if there are no memory
		//  and no valid move assignment operator in class.
		CallUntilSucceed([&] {
			m_cancel = std::move_if_noexcept(cancel);
		});
		if (m_storage.index() == CanceledStateIndex)
		{
			InvokeCancel();
		}
	}

	void Cancel() noexcept
	{
		lock_guard lock(m_mutex);
		if (m_storage.index() == CanceledStateIndex)
		{
			return;
		}
		m_storage = CanceledState();
		if (m_cancel)
		{
			InvokeCancel();
		}
	}

	void Resolve(Value value) noexcept
	{
		lock_guard lock(m_mutex);
		if (m_storage.index() != PendingStateIndex)
		{
			return;
		}
		// Assignment can throw if there are no memory
		//  and no valid move assignment operator in class.
		CallUntilSucceed([&] {
			m_storage = std::move_if_noexcept(value);
		});
		if (m_then)
		{
			InvokeThen();
		}
	}

	void Reject(const std::exception_ptr& exception) noexcept
	{
		lock_guard lock(m_mutex);
		if (m_storage.index() != PendingStateIndex)
		{
			return;
		}
		m_storage = exception;
		if (m_catch)
		{
			InvokeCatch();
		}
	}

private:
	using lock_guard = std::lock_guard<std::mutex>;

	struct PendingState
	{
	};
	struct CanceledState
	{
	};

	using StorageType = std::variant<
		PendingState,
		CanceledState,
		Value,
		std::exception_ptr>;

	// Must match variant type order.
	enum TypeWhich
	{
		PendingStateIndex = 0,
		CanceledStateIndex = 1,
		FulfilledStateIndex = 2,
		RejectedStateIndex = 3,
	};

	template<class Fn>
	void CallUntilSucceed(Fn&& fn) noexcept
	{
		while (true)
		{
			try
			{
				fn();
				return;
			}
			catch (const std::bad_alloc&)
			{
				try
				{
					// Out of memory, sleep until more memory available.
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
				}
				catch (...)
				{
				}
			}
			catch (...)
			{
				// We can't process unexpected exception here
				std::terminate();
			}
		}
	}

	// Called under mutex lock.
	void InvokeThen() noexcept
	{
		CallUntilSucceed([&] {
			if (m_callbackCalled)
			{
				return;
			}

			if (auto dispatcher = m_dispatcher.lock())
			{
				auto sharedThis = this->shared_from_this();
				dispatcher->Dispatch([sharedThis] {
					assert(sharedThis->m_storage.index() == FulfilledStateIndex);
					auto& value = std::get<Value>(sharedThis->m_storage);
					assert(sharedThis->m_then);
					sharedThis->m_then(std::move(value));
				});
				m_callbackCalled = true;
			}
		});
	}

	// Called under mutex lock.
	void InvokeCatch() noexcept
	{
		CallUntilSucceed([&] {
			if (m_callbackCalled)
			{
				return;
			}

			if (auto dispatcher = m_dispatcher.lock())
			{
				auto sharedThis = this->shared_from_this();
				dispatcher->Dispatch([sharedThis] {
					assert(sharedThis->m_storage.index() == RejectedStateIndex);
					auto& exception = std::get<std::exception_ptr>(sharedThis->m_storage);
					assert(sharedThis->m_catch);
					sharedThis->m_catch(exception);
				});
				m_callbackCalled = true;
			}
		});
	}

	// Called under mutex lock.
	void InvokeCancel() noexcept
	{
		CallUntilSucceed([&] {
			if (m_callbackCalled)
			{
				return;
			}

			if (auto dispatcher = m_dispatcher.lock())
			{
				dispatcher->Dispatch(m_cancel);
				m_callbackCalled = true;
			}
		});
	}

	std::mutex m_mutex;
	StorageType m_storage;
	ThenFunction m_then;
	CatchFunction m_catch;
	CancelFunction m_cancel;
	DispatcherWeakPtr m_dispatcher;
	bool m_callbackCalled = false;
};

} // namespace isc
