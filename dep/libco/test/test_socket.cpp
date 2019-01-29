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

#include "co_routine.h"
#include "co_routine_inner.h"

DEFINE_int32(svrroutinecnt, 1, "svr routine cnt");
DEFINE_bool(usesharestack, false, "use copy stack");
DEFINE_int32(sharestackcnt, 10, "copy stack count");
DEFINE_int32(clientcnt, 1, "client cnt");
DEFINE_int32(clithrcnt, 1, "cleint thread cnt");
DEFINE_int32(svrthrcnt, 1, "svr thread cnt");
DEFINE_int32(qps, 1000, "query per second");
DEFINE_int32(active_rate, 100, "active rate");
DEFINE_bool(onlysvr, false, "obly run svr");
DEFINE_bool(onlycli, false, "obly run svr");
DEFINE_string(svrip, "*", "svr ip");


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


using namespace std;

struct stStat_t
{
	uint64_t client_run_conn_cnt;
	uint64_t conn_cnt;
	uint64_t close_cnt;
	uint64_t conn_err_cnt;
	uint64_t conn_succ_time;
	uint64_t conn_err_time;
	uint64_t write_err_cnt;
	uint64_t write_err_time;
	uint64_t read_err_cnt;
	uint64_t read_err_time;

	uint64_t success_req_cnt;
	
	uint64_t total_cost_time;
	uint64_t svr_conn_cnt[10];

	uint64_t svr_write_err_cnt;
	uint64_t svr_read_err_cnt;
	uint64_t svr_total_conn_cnt;
	uint64_t svr_accept_err_cnt;
	uint64_t svr_accept_cnt;

	uint64_t cli_active_cnt;
	uint64_t cli_qps;
};

stStat_t* g_ptStat = NULL;

int SetNonBlock(int fd)
{
	int32_t iFlag = fcntl(fd, F_GETFL);
	if (iFlag < 0)
	{   
		printf("socket %d get F_GETFL err. %d:%s\n", fd, errno, strerror(errno));
		return -1;
	}   

	iFlag |= O_NONBLOCK;

	if (fcntl(fd, F_SETFL, iFlag) < 0)
	{   
		printf("socket %d get F_SETFL err. %d:%s\n", fd, errno, strerror(errno));
		return -1;
	}   
}

void BindSocket(int & fd,  unsigned short port = 0, const char* ip = "*")
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if  (fd < 0)
	{
		printf("create socket error\n");
	}	

	int optval = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,sizeof(int));
	int ret = setsockopt(fd, SOL_SOCKET, 15, (const void *)&optval,sizeof(int));
	if (ret)
	{
		printf("err setsockopt ret %d errno %d %s\n", ret, errno , strerror(errno));
	}
	SetNonBlock(fd);
	sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	if (strcmp(ip, "*") == 0)
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		inet_aton(ip, &(addr.sin_addr));
	}
	addr.sin_port = htons(port);
	ret = bind(fd, (sockaddr*)&addr, sizeof(addr));
	if (ret != 0)
	{
		close(fd);
		printf("bind fd error\n");
	}
	if (listen(fd, 10000) < 0)
	{
		printf("listen error\n");
	}
	return;
}

struct stSvrInfo_t
{
	stCoEvent_t* event;
	stCoCond_t* cond;
	queue<int> waitqueue;
	int svrid;
};
void* OnAccept(int fd, int revent, void* args)
{
	stSvrInfo_t* info = (stSvrInfo_t*)args;
	stCoEvent_t* event = (stCoEvent_t*)info->event;
	if (revent & EPOLLIN)
	{
		sockaddr_in sockAddr;
		socklen_t sockLen = sizeof(sockaddr_in);
		while (true)
		{
			int clifd = co_accept(fd, (sockaddr*)&sockAddr, &sockLen);
			if (clifd > 0)
			{
				//printf("%s:%d recv clifd %d\n", __func__, __LINE__, clifd);
				__sync_fetch_and_add(&g_ptStat->svr_accept_cnt, 1);
				info->waitqueue.push(clifd);
				co_cond_signal(info->cond);
			}
			else
			{
				__sync_fetch_and_add(&g_ptStat->svr_accept_err_cnt, 1);
				break;
			}
		}
	}
	co_add_event(info->event, fd, OnAccept, info, EPOLLIN, -1);
}
void* SvrLogic(int fd);
void* SvrRoutine(void* args)
{
	co_enable_hook_sys();
	stSvrInfo_t* info = (stSvrInfo_t*)args;
	while (true)
	{
		co_cond_timedwait(info->cond, -1);
		while (!info->waitqueue.empty())
		{
			int clifd = info->waitqueue.front();
			info->waitqueue.pop();
			g_ptStat->svr_conn_cnt[info->svrid]++;
			__sync_fetch_and_add(&g_ptStat->svr_total_conn_cnt, 1);
			//printf("recv clifd %d\n", clifd);
			timeval tm;
			tm.tv_sec = 30;
			tm.tv_usec = 0;
			setsockopt(clifd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(timeval));
			setsockopt(clifd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(timeval));
			SvrLogic(clifd);
			__sync_fetch_and_sub(&g_ptStat->svr_total_conn_cnt, 1);
			g_ptStat->svr_conn_cnt[info->svrid--];
			close(clifd);
		}
	}
}

void* SvrLogic(int fd)
{
	while (true)
	{
		char sBuf[1024];
		int ret = read(fd, sBuf, 1024);
		if (ret <= 0)
		{
			__sync_fetch_and_add(&g_ptStat->svr_read_err_cnt, 1);
			return NULL;
		}
		//printf("%s:%d fd %d read ret %d\n", __func__, __LINE__, fd, ret);
		int iWriteLen = 0;
		while (iWriteLen < ret)
		{
			ret = write(fd, sBuf + iWriteLen, ret - iWriteLen);
			if (ret <= 0)
			{
				__sync_fetch_and_add(&g_ptStat->svr_write_err_cnt, 1);
				return NULL;
			}
			if (ret > 0)
			{
				iWriteLen += ret;
			}
		}
	}
	return NULL;
}
static __thread stCoRoutineStat_t* last_stat;
static __thread unsigned long long last = 0;
int stat_idx = 0;
int EventLoopFunc(void* args)
{
	int iThread = *(int*)args;
	unsigned long long now = __GetTickUS();
	if ((now - last) < 1000 * 1000)
	{
		return 0;
	}
	last  = now;
	if (stat_idx != iThread)
	{
		return 0;
	}
	stat_idx++;
	stCoRoutineStat_t* stat = co_get_curr_stat();
	if (!last_stat)
	{
		last_stat = (stCoRoutineStat_t*)calloc(1, sizeof(stCoRoutineStat_t));
	}
#define Get(item) (stat->item - last_stat->item)
	printf("%s:%d thredid %d hit_count %llu not_hit_count %llu\n", 
			__func__, __LINE__, 
			iThread,
			Get(hit_count),
			Get(not_hit_count));

	*last_stat = *stat;
	return 0;
}

void* SvrThread(void* args)
{
	int fd;
	BindSocket(fd, 12319, FLAGS_svrip.c_str());
	stSvrInfo_t* info  = (stSvrInfo_t*)args;
	info->cond = co_cond_alloc();
	info->event = co_alloc_event();
	stCoRoutineAttr_t attr = {0};
	if (FLAGS_usesharestack)
	{
		attr.use_share_stack = 1;
		stStackMemEnv_t* env = co_alloc_stackmemenv(FLAGS_sharestackcnt, 128 * 1024);
		attr.stack_env = env;
	}
	else
	{
		attr.use_share_stack = 0;
	}
	for (int i = 0; i < FLAGS_svrroutinecnt; i++)
	{
		stCoRoutine_t * routine = NULL;
		co_create(&routine, &attr, SvrRoutine, info);
		co_resume(routine);
	}
	int ret = co_add_event(info->event, fd, OnAccept, info, EPOLLIN, -1);
	assert(ret == 0);
	co_eventloop(co_get_epoll_ct(), EventLoopFunc, &info->svrid);
	return NULL;
}

class clsCostTime
{
	public:
		clsCostTime()
		{
			m_stBegin = __GetTickUS();
			m_stLast = m_stBegin;
		}
		unsigned long long CostFromBegin()
		{
			unsigned long long now = __GetTickUS();
			return now - m_stBegin;
		}
		unsigned long long CostFromLast()
		{
			unsigned long long now = __GetTickUS();
			unsigned long long cost = now - m_stLast;
			m_stLast = now;
			return cost;
		}
	private:
		unsigned long long m_stBegin;
		unsigned long long m_stLast;

};

void* ClientRoutine(void* args)
{
	co_enable_hook_sys();
	

	int qps = FLAGS_qps /  (FLAGS_clientcnt *  FLAGS_clithrcnt);
	if (qps <=  0)
	{
		qps = 1;
	}
	int seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	exponential_distribution<double> distribution((double)1.0 / (double)qps);

	sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	if (FLAGS_svrip[0]== '*')
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(FLAGS_svrip.c_str());
	}
	addr.sin_port = htons(12319);
	timeval t;
	t.tv_sec = 10;
	while (true)
	{
		//split
		int connect_split = rand() % 1000 + 1;
		poll(NULL, 0, connect_split);

		int fd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(timeval));
		setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(timeval));

		clsCostTime t;
		__sync_fetch_and_add(&g_ptStat->client_run_conn_cnt, 1);
		int ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
		if (ret)
		{
			printf("%s:%d connect error ret %d errno %d %s\n", __func__, __LINE__, ret, errno ,strerror(errno));
			close(fd);
			unsigned long long conn_err_time = t.CostFromLast();
			__sync_fetch_and_add(&g_ptStat->conn_err_cnt, 1);
			__sync_fetch_and_add(&g_ptStat->conn_err_time, conn_err_time);
			continue;
		}
		__sync_fetch_and_add(&g_ptStat->conn_cnt, 1);
		__sync_fetch_and_add(&g_ptStat->conn_succ_time, t.CostFromLast());

		char sBuf[128];
		snprintf(sBuf, 128, "helloworld");
		//int iBufLen = strlen(sBuf) + 1;
		int iBufLen = 128;
		while (true)
		{
			bool active = (rand() % 100) <= FLAGS_active_rate;
			if (!active)
			{
				poll(NULL, 0, 1000);
				continue;
			}
			__sync_fetch_and_add(&g_ptStat->cli_active_cnt, 1);
			clsCostTime totaluse;
			while (true)
			{
				unsigned long long used = totaluse.CostFromBegin();
				if (used > (1000 * 1000))
				{
					break;
				}
				int iWriteLen = 0;
				while (iWriteLen < iBufLen)
				{
					ret = write(fd, sBuf + iWriteLen, iBufLen - iWriteLen);
					if (ret > 0)
					{
						iWriteLen += ret;
					}
					else
					{
						__sync_fetch_and_sub(&g_ptStat->conn_cnt, 1);
						__sync_fetch_and_add(&g_ptStat->write_err_cnt, 1);
						__sync_fetch_and_add(&g_ptStat->write_err_time, t.CostFromLast());
						close(fd);
						//printf("%s:%d ret %d errno %d %s\n", __func__, __LINE__, ret, errno ,strerror(errno));
						break;
					}
				}
				if (ret <= 0)
				{
					break;
				}
				t.CostFromLast();
				int iReadLen = 0;
				while (iReadLen < iBufLen)
				{
					ret = read(fd, sBuf + iReadLen, iBufLen - iReadLen);
					if (ret > 0)
					{
						iReadLen += ret;
					}
					else
					{
						__sync_fetch_and_sub(&g_ptStat->conn_cnt, 1);
						__sync_fetch_and_add(&g_ptStat->read_err_cnt, 1);
						__sync_fetch_and_add(&g_ptStat->read_err_time, t.CostFromLast());
						//printf("%s:%d ret %d errno %d %s\n", __func__, __LINE__, ret, errno ,strerror(errno));
						close(fd);
						break;
					}
				}
				__sync_fetch_and_add(&g_ptStat->total_cost_time, t.CostFromLast());
				if (ret <= 0)
				{
					break;
				}
				__sync_fetch_and_add(&g_ptStat->success_req_cnt, 1);

				//unsigned long long ullCost = totaluse.CostFromLast() / 1000;
				//int iCurPollTime = 1000 / qps  - ullCost;
			//	printf("%s:%d recv %s\n", __func__, __LINE__, sBuf);
			}
			__sync_fetch_and_sub(&g_ptStat->cli_active_cnt, 1);
			if (ret <= 0)
			{
				break;
			}
		}
	}
	return NULL;
}
int TickFunc(void* args)
{
	int iCount = 0;
	
	while (true)
	{
		sleep(1);
		if (FLAGS_onlysvr)
		{
			continue;
		}
		iCount++;
		printf("%s:%d  success_req_cnt %llu avg_count %llu active_cnt %llu avg_runtime %llu clinet_run_conn_cnt %llu conn_cnt %llu svr_conn_cnt %llu "
				"conn_err_cnt %llu conn_err_time %llu write_err_cnt %llu write_err_time %llu read_err_cnt %llu read_err_time %llu "
				"total_cost_time %llu svr_read_err_cnt %llu svr_write_err_cnt %llu accpet_err_cnt %llu "
				"accept_cnt %llu ",
				__func__, __LINE__, g_ptStat->success_req_cnt,   g_ptStat->success_req_cnt / iCount,
				g_ptStat->cli_active_cnt,
				(g_ptStat->total_cost_time / (g_ptStat->success_req_cnt + 1)),
				g_ptStat->client_run_conn_cnt, 
				g_ptStat->conn_cnt, g_ptStat->svr_total_conn_cnt, 
				g_ptStat->conn_err_cnt, g_ptStat->conn_err_time, 
				g_ptStat->write_err_cnt, g_ptStat->write_err_time,
				g_ptStat->read_err_cnt, g_ptStat->read_err_time,
				g_ptStat->total_cost_time, g_ptStat->svr_read_err_cnt, 
				g_ptStat->svr_write_err_cnt, g_ptStat->svr_accept_err_cnt, 
				g_ptStat->svr_accept_cnt);
		for (int i = 0; i < 10; i++)
		{
			if (g_ptStat->svr_conn_cnt[i] > 0)
			{
				printf("svr_%d %d ", i, g_ptStat->svr_conn_cnt[i]);
			}
			else
			{
				break;
			}
		}
		printf("\n");
	}
	return 0;
}
void* CliThread(void* args)
{
	stCoRoutine_t* routine = NULL;
	stCoRoutineAttr_t attr = {0};
	if (FLAGS_usesharestack)
	{
		attr.use_share_stack = 1;
		stStackMemEnv_t* env = co_alloc_stackmemenv(FLAGS_sharestackcnt, 128 * 1024);
		attr.stack_env = env;
	}
	else
	{
		attr.use_share_stack = 0;
	}
	for (int i = 0; i < FLAGS_clientcnt; i++)
	{
		co_create(&routine, &attr, ClientRoutine, NULL);
		co_resume(routine);
	}
	co_eventloop(co_get_epoll_ct(), NULL, NULL);
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	printf("%s:%d stCoRoutine_t %d\n", __func__, __LINE__, sizeof(stCoRoutine_t));
	SetUseShareStack(true);
	if (!g_ptStat)
	{
		g_ptStat = new stStat_t;
		memset(g_ptStat, 0, sizeof(stStat_t));
	}
	assert ( signal ( SIGPIPE, SIG_IGN ) != SIG_ERR ) ;
	assert ( signal ( SIGCHLD, SIG_IGN ) != SIG_ERR ) ;
	assert ( signal ( SIGALRM, SIG_IGN ) != SIG_ERR ) ;
	google::ParseCommandLineFlags(&argc, &argv, true);

	stSvrInfo_t* svr_info =  new stSvrInfo_t[FLAGS_svrthrcnt];
	if (!FLAGS_onlycli)
	{
		for (int i = 0; i < FLAGS_svrthrcnt; i++)
		{
			pthread_t tid;
			svr_info[i].svrid = i;
			pthread_create(&tid, NULL, SvrThread, svr_info + i);
			pthread_detach(tid);
		}
	}

	if (!FLAGS_onlysvr)
	{
		for (int i = 0; i < FLAGS_clithrcnt; i++)
		{
			pthread_t tid;
			pthread_create(&tid, NULL, CliThread, NULL);
			pthread_detach(tid);
		}
	}

	TickFunc(NULL);
	return 0;
}

//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL: http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/colib/test/test_socket.cpp $ $Id: test_socket.cpp 1651504 2016-06-28 12:44:38Z leiffyli $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

