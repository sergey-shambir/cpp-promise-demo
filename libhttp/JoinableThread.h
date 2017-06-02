#include <thread>

namespace http
{
template<typename Thread>
class JoinableThread
{
public:
	JoinableThread() = default;
	JoinableThread(const JoinableThread&) = default;
	JoinableThread& operator=(const JoinableThread&) = default;

	template<typename T>
	JoinableThread(T&& f)
		: m_thread(std::forward<T>(f))
	{
	}

	JoinableThread(JoinableThread&& x)
		: m_thread(std::move(x.m_thread))
	{
	}

	JoinableThread& operator=(JoinableThread&& x)
	{
		return *this = std::move(x.m_thread);
	}

	JoinableThread& operator=(Thread&& thread)
	{
		if (&m_thread != &thread)
		{
			JoinIfJoinable();
			m_thread = std::move(thread);
		}
		return *this;
	}

	~JoinableThread()
	{
		JoinIfJoinable();
	}

	bool joinable() const
	{
		return m_thread.joinable();
	}

	void join()
	{
		m_thread.join();
	}

	void JoinIfJoinable()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

private:
	Thread m_thread;
};
}
