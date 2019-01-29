#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "coctx.h"
#include "co_routine.h"
#include "co_routine_inner.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
extern "C"
{
	extern void coctx_swap(coctx_t*, coctx_t*) asm("coctx_swap");
	extern void test_xmm(char*, char*) asm("test_xmm");
}


void Test1()
{
	char a16[16] __attribute__ ((aligned (16)))= "0123456789abcde";
	char b16[16]  __attribute__ ((aligned (16))) = "abcde012345789"; 
	test_xmm( a16, b16);
	printf("(%p : %s) (%p : %s) \n", &a16, a16, &b16, b16);
	return;
}

void* RoutineFunc(void* args1, void* args2)
{
	coctx_t* main = (coctx_t*)(args1);
	coctx_t* self = (coctx_t*)args2;
	printf("%s:%d args1 %p args2 %p main %p self %p\n", __func__, __LINE__, &args1, &args2, main ,self);
	coctx_swap(self, main);
	return NULL;
}

void* TestXMMFunc(void* args1, void* args2)
{
	Test1();

	coctx_t* main = (coctx_t*)(args1);
	coctx_t* self = (coctx_t*)args2;
	char s = 's';
	printf("%s:%d args1 %p args2 %p main %p self %p\n", __func__, __LINE__, &args1, &args2, main ,self);

	coctx_swap(self, main);

}


TEST(Coctx, normal)
{
	coctx_t main;
	coctx_init(&main);

	int i = 5;
	coctx_t ctx;
	coctx_init(&ctx);
	ctx.ss_size = 1024 * 128;
	ctx.ss_sp = (char*)malloc(ctx.ss_size);
	coctx_make(&ctx, RoutineFunc, (void*)&main, (void*)&ctx);

	coctx_swap(&main, &ctx);
	return;
}

TEST(Coctx, xmm)
{
	coctx_t main;
	coctx_init(&main);

	coctx_t self;
	coctx_init(&self);
	self.ss_size = 1024 * 128;
	self.ss_sp = (char*)malloc(self.ss_size);
	coctx_make(&self, TestXMMFunc, (void*)&main, (void*)&self);
	coctx_swap(&main, &self);

	printf("%s:%d end\n", __func__, __LINE__);
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
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL: http://scm-gy.tencent.com/gzrd/gzrd_mail_rep/QQMailcore_proj/trunk/basic/colib/test/test_colib.cpp $ $Id: test_colib.cpp 2316881 2017-11-13 07:57:27Z leiffyli $ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

