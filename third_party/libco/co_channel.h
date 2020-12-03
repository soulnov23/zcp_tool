#pragma once
#include "co_routine.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
template <typename T>
class clsCoChannel
{
	public:
		clsCoChannel()
		{
            ref_use = 1;
			
			pop_wait_cond = co_cond_alloc();
			push_wait_cond = co_cond_alloc();
		}
		

        int Kill(int signo)
        {
            co_cond_kill(pop_wait_cond, signo);
            return 0;
        }
		int SyncPush(T val)
		{
			while (co_cond_empty(pop_wait_cond))
			{
				co_cond_timedwait(push_wait_cond, -1);
			}
			q.push(val);
			co_cond_signal(pop_wait_cond);
			return 0;
		}
        int Push(T val)
        {
            q.push(val);
			co_cond_signal(pop_wait_cond);
            return 0;
        }
		int PushToWait(T val)
		{
			if (co_cond_empty(pop_wait_cond))
			{
				return -1;
			}
			q.push(val);
			co_cond_signal(pop_wait_cond);
			return 0;
		}

        bool IsPopWaitEmpty()
        {
			return co_cond_empty(pop_wait_cond);
        }

		int SyncPop(T* val)
		{
            int wait_signal = 0;
			while (q.empty())
			{
				co_cond_signal(push_wait_cond);
				co_cond_timedwait(pop_wait_cond, -1, &wait_signal);
                if (wait_signal != 0)
                {
                    return wait_signal;
                }
			}
			*val = q.front();
			q.pop();
			return 0;
		}
        int SyncPopAll(std::queue<T>* val)
        {
            while (q.empty())
            {
				co_cond_signal(push_wait_cond);
				co_cond_timedwait(pop_wait_cond, -1);
            }
            *val = q;
            q.clear();
            return 0;
        }
        int Ref()
        {
            ref_use++;
            return 0;
        }
        int UnRef()
        {
            ref_use--;
            if (ref_use == 0)
            {
                while (!q.empty())
                {
                    T p = q.front();
                    delete p;
                    q.pop();
                }
                delete this;
            }
            return 0;

        }
        ~clsCoChannel()
		{
			co_cond_free(push_wait_cond);
			co_cond_free(pop_wait_cond);
		}
	private:
        int ref_use;
		std::queue<T> q;
		stCoCond_t* pop_wait_cond;
		stCoCond_t* push_wait_cond;
};

