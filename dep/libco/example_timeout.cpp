#include "co_routine.h"
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <functional>
#include <vector>
using namespace std;
void* RoutineFunc(void* args) {
	co_enable_hook_sys();
	int* i = (int*)args;
	int timeout = 40 * (*i + 1) * 1000;
	printf("%d before yield timeout %d\n", *i, timeout);
	poll(NULL, 0, timeout);
	printf("%d after yield timeout %d\n", *i, timeout);
	return NULL;
}
int main(int argc, char* argv[]) {
	vector<stCoRoutine_t*> co_pool;
	int a[10];
	stCoRoutine_t* co = NULL;
	for (int i = 0; i < 10; i++) {
		a[i] = i;
		co_create(&co, NULL, RoutineFunc, &a[i]);
		co_resume(co);
	}
	co_eventloop(co_get_epoll_ct(), NULL, NULL);
}

//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

