#include "atomic_count.h"
#include "thread.h"
using namespace zcp_tool;
#include <iostream>
using namespace std;

void *inc_ac(void *arg)
{
	atomic_count *ac = (atomic_count*)arg;
	for(int i = 0; i < 10000; ++i)
		++(*ac);
}

void *dec_ac(void *arg)
{
	atomic_count *ac = (atomic_count*)arg;
	for(int i = 0; i < 10000; ++i)
		--(*ac);
}

int main(int argc, char *argv[])
{
	atomic_count ac(0);
	thread t1;
	thread t2;
	thread t3;
	thread t4;
	if (!t1.create(inc_ac, (void*)(&ac)))
	{
		cout << "create thread failed" << endl;
	}
	if (!t2.create(dec_ac, (void*)(&ac)))
	{
		cout << "create thread failed" << endl;
	}
	if (!t3.create(dec_ac, (void*)(&ac)))
	{
		cout << "create thread failed" << endl;
	}
	if (!t4.create(dec_ac, (void*)(&ac)))
	{
		cout << "create thread failed" << endl;
	}
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	cout << ac << endl;
	return 0;
}

