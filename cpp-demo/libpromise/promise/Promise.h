#pragma once
#include "IPromise.h"
#include "../platform/IDispatcher.h"
#include <atomic>
#include <mutex>
#include <boost/variant.hpp>
#include <boost/noncopyable.hpp>
#include "promise_detail.h"

namespace isprom
{
template <class ValueType>
class Promise
        : public IPromise<ValueType>
        , public std::enable_shared_from_this<Promise<ValueType>>
        , private boost::noncopyable
{
public:
    using ThenFunction = typename IPromise<ValueType>::ThenFunction;
    using CatchFunction = typename IPromise<ValueType>::CatchFunction;

    Promise(IDispatcher &dispatcher)
        : m_dispatcher(dispatcher)
        , m_canceled(false)
    {
    }

    void Then(const ThenFunction &onFulfilled) override
    {
        lock_guard lock(m_mutex);
        if (m_then)
        {
            throw std::logic_error("Cannot call Then twice (implementation limitation)");
        }

        // Модный способ работы с boost.variant: switch/case по which,
        //  which-индекс извлекается с помощью Boost MPL.
        switch (m_storage.which())
        {
        case ISPROM_WHICH(StorageType, PendingState):
            m_then = onFulfilled;
            break;
        case ISPROM_WHICH(StorageType, ValueType):
            m_then = onFulfilled;
            InvokeThen();
            break;
        default:
            break;
        }
    }

    void Catch(const CatchFunction &onRejected) override
    {
        lock_guard lock(m_mutex);
        if (m_catch)
        {
            throw std::logic_error("Cannot call Catch twice (implementation limitation)");
        }

        // Модный способ работы с boost.variant: switch/case по which,
        //  which-индекс извлекается с помощью Boost MPL.
        switch (m_storage.which())
        {
        case ISPROM_WHICH(StorageType, PendingState):
            m_catch = onRejected;
            break;
        case ISPROM_WHICH(StorageType, std::exception_ptr):
            m_catch = onRejected;
            InvokeCatch();
            break;
        default:
            break;
        }
    }

    void Cancel() override
    {
        {
            lock_guard lock(m_mutex);
            if (m_storage.which() == ISPROM_WHICH(StorageType, CanceledTag))
            {
                return;
            }
            m_canceled = true;
            m_storage = CanceledTag();
        }
        OnCancel();
    }

    bool IsCanceled()
    {
        return m_canceled;
    }

    void Resolve(ValueType && value)
    {
        CheckMovable<ValueType>();
        lock_guard lock(m_mutex);
        if (m_storage.which() != ISPROM_WHICH(StorageType, PendingState))
        {
            return;
        }
        m_storage = std::move(value);
        if (m_then)
        {
            InvokeThen();
        }
    }

    void Reject(std::exception_ptr && exception)
    {
        CheckMovable<std::exception_ptr>();
        lock_guard lock(m_mutex);
        if (m_storage.which() != ISPROM_WHICH(StorageType, PendingState))
        {
            return;
        }
        m_storage = std::move(exception);
        if (m_catch)
        {
            InvokeCatch();
        }
    }

protected:
    // Allows subclasses to have custom behavior on cancel.
    virtual void OnCancel() {}

private:
    using lock_guard = std::lock_guard<std::mutex>;
    struct CanceledTag {};
    struct PendingState {};

    using StorageType = boost::variant<
        PendingState,
        CanceledTag,
        ValueType,
        std::exception_ptr
    >;

    template<class T>
    inline void CheckMovable()
    {
        static_assert(std::is_nothrow_move_constructible<T>::value,
                      "type should be nonthrow move constructible");
        static_assert(std::is_nothrow_move_assignable<T>::value,
                      "type should be nonthrow move assignable");
    }

    void InvokeThen()
    {
        auto sharedThis = this->shared_from_this();
        m_dispatcher.Post([sharedThis] {
            auto &value = boost::get<ValueType>(sharedThis->m_storage);
            sharedThis->m_then(std::move(value));
        });
    }

    void InvokeCatch()
    {
        auto sharedThis = this->shared_from_this();
        m_dispatcher.Post([sharedThis] {
            auto &exception = boost::get<std::exception_ptr>(sharedThis->m_storage);
            sharedThis->m_catch(exception);
        });
    }

    IDispatcher &m_dispatcher;
    std::atomic_bool m_canceled;
    std::mutex m_mutex;
    StorageType m_storage;
    ThenFunction m_then;
    CatchFunction m_catch;
};
}
