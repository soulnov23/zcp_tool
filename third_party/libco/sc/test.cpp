#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
static int g_cnt = 0;
typedef int (*coctx_pfn_t)( const char *s,const char *s2 );
struct coctx_param_t
{
	coctx_pfn_t f;
	coctx_pfn_t f_link;
	const void *s1;
	const void *s2;
};
struct coctx_t
{
	void *regs[ 5 ];

	coctx_param_t *param;

	coctx_pfn_t routine;
	const void *s1;
	const void *s2;
	size_t ss_size;
	char *ss_sp;
	
};
#define ESP 0
#define EIP 1
// -----------
#define RSP 0
#define RIP 1
#define RBX 2
#define RDI 3
#define RSI 4

coctx_param_t *b = NULL; 
coctx_t *o = 0;
coctx_t *n = 0;

extern "C"
{
	extern void coctx_swap( coctx_t *,coctx_t* ) asm("coctx_swap");
};
#if defined(__i386__)
static int coctx_init( coctx_t *ctx )
{
	memset( ctx,0,sizeof(*ctx));
	return 0;
}
static int coctx_make( coctx_t *ctx,coctx_pfn_t pfn,const void *s,const void *s1 )
{

	char *sp = ctx->ss_sp + ctx->ss_size ;
	sp = (char*)((unsigned long)sp & -16L); 

	int len = sizeof(coctx_param_t) + 64;
	memset( sp - len,0,len );
	ctx->routine = pfn;
	ctx->s1 = s;
	ctx->s2 = s1;

	ctx->param = (coctx_param_t*)sp ;
	ctx->param->f = pfn;	
	ctx->param->f_link = 0;
	ctx->param->s1 = s;
	ctx->param->s2 = s1;

	n->regs[ ESP ] = (char*)(ctx->param) + sizeof(void*);
	n->regs[ EIP ] = (char*)pfn;


	return 0;
}
#elif defined(__x86_64__)
static int coctx_make( coctx_t *ctx,coctx_pfn_t pfn,const void *s,const void *s1 )
{
	char *stack = ctx->ss_sp;
	*stack = 0;

	char *sp = stack + ctx->ss_size - 1;
	sp = (char*)( ( (unsigned long)sp & -16LL ) - 8);

	int len = sizeof(coctx_param_t) + 64;
	memset( sp - len,0,len );

	ctx->routine = pfn;
	ctx->s1 = s;
	ctx->s2 = s1;

	ctx->param = (coctx_param_t*)sp;
	ctx->param->f = pfn;	
	ctx->param->f_link = 0;
	ctx->param->s1 = s;
	ctx->param->s2 = s1;

	n->regs[ RBX ] = stack + ctx->ss_size - 1;
	n->regs[ RSP ] = (char*)(ctx->param) + 8;
	n->regs[ RIP ] = (char*)pfn;

	n->regs[ RDI ] = (char*)s;
	n->regs[ RSI ] = (char*)s1;

	return 0;
}
static int coctx_init( coctx_t *ctx )
{
	memset( ctx,0,sizeof(*ctx));
	return 0;
}

#endif

int test( const char *s,const char *s2 )
{
	int a = 0;
	int c = 0;

	for(int i=0;i<g_cnt;i++)
	{
		//printf("in routine %d [%s][%s]\n",i,s,s2);
		coctx_swap( n,o );
	}
	return 0;
}
unsigned long long gt()
{
	struct timeval s;
	gettimeofday( &s,0 );

	unsigned long long l = s.tv_sec;
	l *= 1000;
	l += s.tv_usec / 1000;
	return l;
}
int main(int argc,char *argv[])
{
	g_cnt = atoi( argv[1] );
	int stack_size = 2048;
	char *stack = (char*)malloc( stack_size );

	o = (coctx_t*)malloc( sizeof(coctx_t) );
	n = (coctx_t*)malloc( sizeof(coctx_t) );

	coctx_init( o );
	coctx_init( n );

	n->ss_sp = stack;
	n->ss_size = stack_size;

	coctx_make( n,test,"sunny_param","sunny_param2" );

	b = o->param;

	unsigned long long s = gt();
	for(int i=0;i<g_cnt;i++)
	{
		coctx_swap( o,n );
	}
	unsigned long long u = gt() - s;

	printf("end %lld ms\n",u);
	return 0;
}


