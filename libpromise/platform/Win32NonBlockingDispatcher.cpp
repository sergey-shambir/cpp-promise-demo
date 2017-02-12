#include "stdafx.h"
#include "Win32NonBlockingDispatcher.h"
#include <queue>
#include <mutex>
#include <boost/optional.hpp>

#if defined(_WIN32)

// WTL includes
#define NOMINMAX
#include <Windows.h>
#include <atlbase.h>
#include <atlapp.h>


namespace isprom
{
struct Win32NonBlockingDispatcher::Impl
	: public ATL::CWindowImpl<Impl>
{
	using mutex_lock = std::lock_guard<std::mutex>;

	enum
	{
		WM_DO_WORK = WM_USER + 1
	};

	Impl()
	{
		Create(HWND_MESSAGE);
	}

	~Impl()
	{
		DestroyWindow();
	}

	void Post(const Operation &operation)
	{
		{
			mutex_lock lock(m_tasksMutex);
			m_tasks.push(operation);
		}
        ATLVERIFY(PostMessage(WM_DO_WORK));
	}

	BEGIN_MSG_MAP(Impl)
		MESSAGE_HANDLER(WM_DO_WORK, OnDoWork)
	END_MSG_MAP()
	
private:
	LRESULT OnDoWork(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DoWorkImpl();
		return 0;
	}

	void DoWorkImpl()
	{
        // Reading count before execution protects us
        //  from "add-execute-add-execute-..." infinitive loop.
		unsigned operationsCount = ReadOperationsCount();
		while (operationsCount--)
		{
			Operation operation = PopOperation();
			try
			{
				assert(operation);
				operation();
			}
			catch (...)
			{
			}
		}
	}

	size_t ReadOperationsCount()
	{
		// FIXME: we can avoid this extra lock.
		mutex_lock lock(m_tasksMutex);
		return m_tasks.size();
	}

	Operation PopOperation()
	{
		mutex_lock lock(m_tasksMutex);
		assert(!m_tasks.empty());
		Operation operation = m_tasks.back();
		m_tasks.pop();
		return operation;
	}

	std::queue<Operation> m_tasks;
	std::mutex m_tasksMutex;
};

Win32NonBlockingDispatcher::Win32NonBlockingDispatcher()
	: m_impl(std::make_unique<Impl>())
{
}

Win32NonBlockingDispatcher::~Win32NonBlockingDispatcher()
{
}

void Win32NonBlockingDispatcher::Post(const Operation &operation)
{
	m_impl->Post(operation);
}

}

#endif
