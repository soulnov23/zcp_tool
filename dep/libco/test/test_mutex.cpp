#include "co_comm.h"
#include "co_routine_pool.h"
int main(int argc, char* argv[]) 
{
	clsCoMutex m;
	clsRoutinePool pool(10, NULL);
	int i = 0; 
	for (int i = 0; i< 9; i++)
	{
		pool.Run([&](int workeridx ) {
				co_enable_hook_sys();
				for (int i = 0 ;i < 10; i++)
				{
					m.CoLock();
					poll(NULL, 0, 1000);
					printf("%s:%d workeridx %d lock %p\n", __func__, __LINE__, 
						workeridx, co_self());
					m.CoUnLock();
				}
			}
		);
	}
	co_eventloop(co_get_epoll_ct(), NULL, NULL);

}

//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

