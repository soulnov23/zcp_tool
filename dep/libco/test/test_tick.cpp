#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>
#include <queue>
#include <assert.h>
#include <poll.h>
#include <gflags/gflags.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <iomanip>
#include <random>
#include <sys/time.h>
#include <map>
#include <sys/epoll.h>
#include <poll.h>

#include <errno.h>
#include "co_routine.h"

static unsigned long long counter(void)
{
#ifdef NORDTSCP
	uint32_t lo, hi;
	unsigned long long o;
	__asm__ __volatile__ (
			"rdtsc" : "=a"(lo), "=d"(hi)
			);
	o = hi;
	o <<= 32;
	return (o | lo);
#else
	uint32_t lo, hi;
	unsigned long long o;
	__asm__ __volatile__ (
			"rdtscp" : "=a"(lo), "=d"(hi)
			);
	o = hi;
	o <<= 32;
	return (o | lo);
#endif
}
static unsigned long long GetCpuHz()
{
	FILE *fp = fopen("/proc/cpuinfo","r");
	if(!fp) return 1;
	char buf[4096] = {0};
	fread(buf,1,sizeof(buf),fp);
	fclose(fp);

	char *lp = strstr(buf,"cpu MHz");
	if(!lp) return 1;
	lp += strlen("cpu MHz");
	while(*lp == ' ' || *lp == '\t' || *lp == ':')
	{
		++lp;
	}

	double mhz = atof(lp);
	unsigned long long u = (unsigned long long)(mhz);
	return u;
}
unsigned long long __GetTickS()
{
	static uint64_t hz = GetCpuHz() * 1000 * 1000;
	return counter() / hz ;
}
unsigned long long __GetTickUS()
{
	static uint64_t hz = GetCpuHz();
	return counter() / hz;
}
void* ThreadFunc(void* args)
{
	int threadid = *(int*)args;
	static __thread unsigned long long last = 0;
	last = __GetTickUS();
	volatile unsigned long long now;
	for (int i = 0; i < 1000 * 1000 * 1000; i++)
	{
		now =  __GetTickUS();
		//printf("tid %d last %llu now %llu\n", threadid, last, now);
	}
	now = __GetTickUS();
	printf("%s:%d used %llu\n", __func__, __LINE__, now - last);

	timeval last_tm;
	gettimeofday(&last_tm, NULL);
	timeval now_tm;
	unsigned long long used;
	for (int i = 0; i < 1000 * 1000 * 1000; i++)
	{
		gettimeofday(&now_tm, NULL);
		used = now_tm.tv_sec * 1000 * 1000 + now_tm.tv_usec;
	}
	gettimeofday(&now_tm, NULL);
	printf("%s:%d gettimeofday used %llu used1 %llu\n", __func__, __LINE__, (now_tm.tv_sec * 1000 * 1000 + now_tm.tv_usec) 
			- (last_tm.tv_sec * 1000 * 1000 - last_tm.tv_usec), used);

}

const char* filename = "/dev/urandom";
void* TestRoutine(void* args)
{
	
	co_enable_hook_sys();
	char sBuf[16];
	int fd = open(filename, O_RDONLY);

	struct pollfd pset;
	pset.fd = fd;
	pset.events = POLLIN;
	pset.revents = 0;
	
	int ret = poll(&pset, 1, 1000);

	if (ret > 0)
	{
		ret = read(fd, sBuf, 16);
		sBuf[15] = '\0';
		printf("%s:%d read ret %d\n", __func__, __LINE__, ret);
	}
	else
	{
		printf("%s:%d read  ret %d errno %d %s\n", 
				__func__, __LINE__, ret, errno, strerror(errno));
	}

}
int main(int argc, char* argv[])
{
	/*
	   int threadcnt = atoi(argv[1]);
	   int* threadid = new int[threadcnt +1];
	   for (int i = 0; i < threadcnt; i++)
	   {
	   threadid[i] = i;
	   pthread_t tid;
	   pthread_create(&tid, NULL, ThreadFunc, &threadid[i]);
	   }
	   threadid[threadcnt] = threadcnt;
	 */

	/* test tick */
#if 0
	int threadid[2];
	int threadcnt=1;
	ThreadFunc((void*)&threadid[threadcnt]);
	char sBuf[16];
	int fd = open(filename, O_RDONLY|O_NONBLOCK);

	struct pollfd pset;
	pset.fd = fd;
	pset.events = POLLIN;
	pset.revents = 0;
	
	
	int ret = poll(&pset, 1, 1000 * 10);
	if (ret > 0)
	{
		ret = read(fd, sBuf, 16);
		sBuf[15] = '\0';
		printf("%s:%d read ret %d\n", __func__, __LINE__, ret);
	}
	else
	{
		printf("%s:%d read  ret %d errno %d %s\n", 
				__func__, __LINE__, ret, errno, strerror(errno));
	}
#endif

	int fd = open(filename, O_RDONLY);
	int epfd = epoll_create(1024);
	epoll_event epevent[1024];
	epoll_event cur;
	cur.events = EPOLLIN;
	cur.data.fd = fd;

	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &cur);
	if (ret)
	{
		printf("epoll_ctl add ret %d errno %d %s\n", ret, errno, strerror(errno));
	}

	stCoRoutine_t* routine = NULL;
	co_create(&routine, NULL, TestRoutine, NULL);
	co_resume(routine);
	co_eventloop(co_get_epoll_ct(), NULL, NULL);
	return 0;
}



//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL: http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/colib/test/test_tick.cpp $ $Id: test_tick.cpp 1672085 2016-07-14 08:08:29Z leiffyli $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

