#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <error.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include "co_channel.h"

#include "co_comm.h"
#include "co_routine_lambda.h"

int TickFunc(void* args) 
{
	int* i = (int*)args;
	return *i;
}

TEST(TESTComm, TestConnection)
{
	signal ( SIGPIPE, SIG_IGN);
	int fd[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);

	clsCoConnection* conn[2];
	
	int e = 0;
	
	int len = 1024 * 1024 * 4; 
	char* buffer = new char[1024  * 1024 * 4];
	snprintf(buffer, 128, "hello world\n");
	int i = 0;
	for (int i = 0; i< 2; i++)
	{
		conn[i] = new clsCoConnection(fd[i]);
		stCoRoutine_t* conn1_read_co = co_create(NULL, [i, conn, len, &buffer, fd] {
				co_enable_hook_sys();
				for (int j = 0; j < 10; j++) 
				{
					int ret = conn[i]->Write(buffer, len);
					if (ret < 0)
					{
						printf("%s:%d write ret %d errno %d %s\n", 
							__func__, __LINE__, ret, errno ,strerror(errno));
						break;
					}
					poll(NULL, 0, 500);
				}
				close(fd[0]);
				close(fd[1]);
				printf("%s:%d close %d\n", __func__, __LINE__, i);
			});

		stCoRoutine_t* conn1_write_co = co_create(NULL, [i, conn, len, &e] {
				co_enable_hook_sys();
				while (true)
				{
					char* buffer = new char[len];
					int ret = conn[i]->Read(buffer, len);
					if (ret < 0)
					{
						printf("%s:%d i %d read ret %d errno %d %s\n", 
							__func__, __LINE__, i, ret, errno ,strerror(errno));
						break;
					}
					else if (ret > 0)
					{
						printf("conn %d ret %d\n", i, ret);
					}
				}
				e = -1;
			});
	}
	co_eventloop(co_get_epoll_ct(), TickFunc, &e);
}

TEST(TESTComm, TestPollSignal)
{
	stCoCond_t* p[10];
	int e = 0;
	for (int i = 0; i < 10; i++)
	{
		p[i] = co_cond_alloc();
	}

	stCoRoutine_t* co = co_create(NULL, [=]{
		while (true)
		{
			stPollCond_t pfd[10];
			for (int i = 0; i < 10; i++)
			{
				pfd[i].cond = p[i];
			}
			co_cond_poll(pfd, 10, -1);
			for (int i = 0; i < 10; i++)
			{
				if (pfd[i].active == true)
				{
					printf("%s:%d signal %d active\n", 
						__func__, __LINE__, i);
				}
			}
		}
	});
	stCoRoutine_t* signal_co = co_create(NULL, [&]{
			co_enable_hook_sys();
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < i; j++)
				{
					co_cond_signal(p[j]);
					printf("signal %d\n", j);
				}
				printf("wait \n");
				poll(NULL, 0, 1000);
			}
			e = -1;
		});

	co_eventloop(co_get_epoll_ct(), TickFunc, &e);
}
TEST(TESTComm, TestPollConnection)
{
	int fd[10][2];
	int e = 0;
	clsCoConnection* conn[10][2];
	for (int i = 0; i < 10; i++)
	{
		ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd[i]), 0);
		conn[i][0] = new clsCoConnection(fd[i][0]);
		conn[i][1] = new clsCoConnection(fd[i][1]);
	}
	stCoRoutine_t* co = co_create(NULL, [&](){
		co_enable_hook_sys();
		for (int i = 0; i < 10; i++)
		{
			stPollEntry_t entry[10];
			for (int i = 0; i < 10; i++)
			{
				entry[i].conn = conn[i][0];
				entry[i].mode = eWaitRead;
			}
			int ret = clsPollConnection::Poll(entry, 10, -1);
			for (int i = 0; i < 10; i++)
			{
				if (entry[i].rmode == eWaitRead)
				{
					printf("%s:%d entry %d rmode waitread\n", __func__, __LINE__, i);
					while (true)
					{
						char buf[20];
						int ret = conn[i][0]->Read(buf, 20, true);
						if (ret > 0)
						{
							printf("read conn %d buf %s", i, buf);
						}
						else
						{
							break;
						}
					}
				}
			}
		}
			return;
	});
	stCoRoutine_t* co1 = co_create(NULL, [&]() {
		co_enable_hook_sys();
		char buf[10];
		snprintf(buf, 10, "haha\n");
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				conn[j][1]->Write(buf, 10);
				printf("write conn %d\n", j);
			}
			poll(NULL, 0, 1000);
		}
		e = -1;
	});

	co_eventloop(co_get_epoll_ct(), TickFunc, &e);

	for (int i = 0; i < 10; i++)
	{
		ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd[i]), 0);
		delete conn[i][0];
		delete conn[i][1];
	}
	
}

void* OnRead(int fd, int revent, void* args)
{
	clsCoConnection* conn = (clsCoConnection*)args;
	printf("%s:%d onread revent %d fd %d\n", __func__, __LINE__, revent, fd);
	if (revent & EPOLLIN)
	{
		while (true)
		{
			char buf[128];
			int ret = conn->Read(buf, 128, true);
			if (ret < 0)
			{
				break;
			}
			else if (ret == 0)
			{
				printf("read close %d\n", ret);
				break;
			}
			printf("%s:%d read %d\n", __func__, __LINE__, ret);
		}
	}
	conn->SetReadEventCallBackOneShot(OnRead, conn, 2 * 1000);
	return NULL;
}
TEST(TESTComm, TestConnReadEvent)
{
	int fd[2];
	clsCoConnection* conn[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);
	conn[0] = new clsCoConnection(fd[0]);
	conn[1] = new clsCoConnection(fd[1]);

	int e = 4;
	conn[0]->SetReadEventCallBackOneShot(OnRead, conn[0], 2 * 1000);

	stCoRoutine_t* co = co_create(NULL, [&](){
			co_enable_hook_sys();
			while (--e >= -1)
			{
				poll(NULL, 0, 3000);
				char buf[10];
				conn[1]->Write(buf, 10);
			}
			delete conn[1];
			close(fd[1]);
		});


	co_eventloop(co_get_epoll_ct(), TickFunc, &e);
}
TEST(TESTComm, TestPollAndReadMeanwhile)
{
	int e = 1;
	int fd[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);
	clsCoConnection* conn[2];
	conn[0] = new clsCoConnection(fd[0]);
	conn[0]->SetSockTimeout(10 * 1000);
	conn[1] = new clsCoConnection(fd[1]);
	stCoRoutine_t* read_co = co_create([&]{
			co_enable_hook_sys();
			char buf[128];
			int ret = conn[0]->Read(buf, 128);
			if (ret > 0)
			{
				printf("read_co read ret %d buf %s\n", 
					ret, buf);
			}
			else
			{
				printf("read_co read ret %d\n", ret);
			}
			e--;
	});
	stCoRoutine_t* poll_co = co_create([&]{
			co_enable_hook_sys();
			stPollEntry_t entry;
			entry.conn = conn[0];
			entry.mode = eWaitRead;
			int ret = clsPollConnection::Poll(&entry, 1, -1);
			if (ret > 0)
			{
				printf("poll_co ret %d rmode %d\n", 
					ret, entry.rmode);
			}
			else
			{
				printf("poll_co ret %d\n", ret);
			}
			e--;
	});
	stCoRoutine_t* write_co = co_create([&]{
			co_enable_hook_sys();
			printf("before write co\n");
			poll(NULL, 0, 1000);
			printf("begin write co\n");
			char buf[128];
			snprintf(buf, 128, "hello world");
			int ret = conn[1]->Write(buf, 128);
			printf("write_co write ret %d %s\n", ret, buf);
			return;
	});
	co_eventloop(co_get_epoll_ct(), TickFunc, &e);
}

TEST(TESTComm, TestRpc)
{
	int e = 1;
	int fd[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);
	clsCoConnection* conn[2];
	conn[0] = new clsCoConnection(fd[0]);
	conn[0]->SetSockTimeout(10 * 1000);
	conn[1] = new clsCoConnection(fd[1]);

	clsCoChannel<char*> resp_channel(0);
	stCoRoutine_t* client_co = co_create([&]{
		co_enable_hook_sys();
		while (true)
		{
			char buf[128];
			snprintf(buf, 128, "hello world");
			int ret = conn[1]->Write(buf, 128);
			char* psResp = NULL;
			ret = resp_channel.SyncPop(&psResp);
			if (!ret)
			{
				printf("client co recv %s\n", psResp);
				free(psResp);
			}
			else
			{
				printf("clinet co resp ret %d\n", ret);
			}
			e--;
		}

	});
	stCoRoutine_t* read_co = co_create([&]{
			co_enable_hook_sys();
			while (true)
			{
				char* buf = (char*)malloc(128);
				int ret = conn[1]->Read(buf, 128);
				if (ret > 0)
				{
					printf("read_co read ret %d buf %s\n", ret, buf);
					resp_channel.PushToWait(buf);
				}
				else if (ret == -1 && errno == EAGAIN)
				{
					continue;
				}
				else
				{
					printf("read_co read ret %d errno %d %s\n", ret, errno , strerror(errno));
					break;
				}
			}
	});
	stCoRoutine_t* svr_co = co_create([&]{
		co_enable_hook_sys();
		while (true)
		{
			poll(NULL, 0, 2000);
			char buf[128];
			snprintf(buf, 128, "hello world");
			int ret = conn[0]->Read(buf, 128);
			if (ret > 0)
			{
				ret= conn[0]->Write(buf, 128);
				if (ret <= 0)
				{
					printf("svr co write ret %d\n", ret);
				}
			}
			else if (ret == -1 && errno == EAGAIN)
			{
				continue;
			}
			else
			{
				printf("svr co read ret %d\n", ret);
				break;
			}
		}
	});
	co_eventloop(co_get_epoll_ct(), TickFunc, &e);
}
TEST(TESTComm, TestLink)
{
    stListRoot root;
    stClientNode_t clinode[10];
    int a[10];
    for (int i = 0; i < 10; i++)
    {
        a[i] = i;
        clinode[i].value = &a[i];
        AddTail(&root, &clinode[i].timeout_node);
    }
    PrintLinkList(&root);
}


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

