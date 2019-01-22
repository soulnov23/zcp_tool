#include "co_routine.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>

int g_iTick = 0;
stCoEvent_t* timer = NULL;
void* OnTimeout(int fd, int revent, void* args) 
{
	g_iTick++;
	printf("%s:%d timeout tick %d\n", __func__, __LINE__, g_iTick);
	return 0;
}
int TickFunc(void* args)
{
	if (g_iTick == 10) 
	{
		printf("%s:%d tick %d exit\n", __func__, __LINE__, g_iTick);
		co_free_event(timer);
		return -1;
	}
	return 0;
}
int main() 
{
	timer = co_new_timer(OnTimeout, NULL, 1 * 1000, true);
	co_eventloop(co_get_epoll_ct(), TickFunc, NULL);
	return 0;
}
