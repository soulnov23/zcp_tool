#include "thread.h"

namespace zcp_tool
{

thread::thread()
{
	m_thread_t = 0;
}

thread::~thread()
{
	m_thread_t = 0;
}

bool thread::create(THREAD_FUNC thread_func, void *arg)
{
	bool ret = false;
	if (0 == pthread_create(&m_thread_t, NULL, thread_func, arg))
	{
		ret = true;
	}
	else
	{
		m_thread_t = 0;
	}
	return ret;
}

pthread_t thread::get_id()
{
	return m_thread_t;
}

void thread::join()
{
	pthread_join(m_thread_t, NULL);
	m_thread_t = 0;
}

}