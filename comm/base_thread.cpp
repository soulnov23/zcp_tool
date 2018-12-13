#include "base_thread.h"

base_thread::base_thread()
{
	m_thread_t = 0;
}

base_thread::~base_thread()
{
	m_thread_t = 0;
}

bool base_thread::create()
{
	bool ret = false;
	if (0 == pthread_create(&m_thread_t, NULL, thread_func, this))
	{
		ret = true;
	}
	else
	{
		m_thread_t = 0;
	}
	return ret;
}

void base_thread::wait()
{
	pthread_join(m_thread_t, NULL);
	m_thread_t = 0;
}

void *base_thread::thread_func(void *arg)
{
	base_thread *pthis = static_cast<base_thread*>(arg);
	pthis->run();
	return NULL;
}