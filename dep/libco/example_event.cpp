#include "co_routine.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
static bool g_bExit = false;
void* OnRead(int fd, int revent, void* args) 
{
	stCoEvent_t* event = (stCoEvent_t*)args;
	int ret = 0;
	if (revent == EPOLLIN) 
	{
		char buf[128];
		ret = read(fd, buf, 128);
		if (ret > 0) 
		{
			printf("%s:%d recv recv buf len %d buf %s\n", __func__, __LINE__, ret, buf);
			return NULL;
		} 
		else if (ret < 0 && errno == EAGAIN) 
		{
			return NULL;
		}

	}
	//timeout or read fail;
	printf("%s:%d revent ret %d may timeout\n", __func__, __LINE__, ret);
	g_bExit = true;
	close(fd);
	co_free_event(event);
	return NULL;

}
void* OnWrite(int fd, int revent, void* args) 
{
	stCoEvent_t* event = (stCoEvent_t*)args;
	if (revent == EPOLLOUT) 
	{
		const char* sBuf = "hello world";
		int len = strlen(sBuf) + 1;
		int ret = write(fd, sBuf, len);
		if (ret > 0) 
		{
			printf("%s:%d write buf len %d buf %s\n", __func__, __LINE__, ret, sBuf);
			return NULL;
		} 
		else if (ret < 0 && errno == EAGAIN) 
		{
			return NULL;
		}

	}
	//error 
	close(fd);
	co_free_event(event);
	return NULL;

}
int TickFunc(void* args) 
{
	if (g_bExit) 
	{
		return -1;
	}
	return 0;
}
int main(int argc, char* argv[]) 
{
	int pfd[2];
	pipe(pfd);
	stCoEvent_t* r_event = co_alloc_event();
	int ret  = co_add_event(r_event, pfd[0], OnRead, r_event, 
				EPOLLIN, 2000, true);
	if (ret) 
	{
		printf("%s:%d add event err ret %d\n", 
				__func__, __LINE__, ret);
		return -1;
	}

	stCoEvent_t* w_event = co_alloc_event();
	ret = co_add_event(w_event, pfd[1], OnWrite, w_event,
			EPOLLOUT, 1000, false);
	if (ret) 
	{
		printf("%s:%d add event err ret %d\n", 
				__func__, __LINE__, ret);
		return -1;
	}
	co_eventloop(co_get_epoll_ct(), TickFunc, NULL);
	return 0;

}
