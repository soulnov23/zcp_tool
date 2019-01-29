#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
typedef int (*pfn_t)( const char *s,const char *s2 );
struct param_t
{
	pfn_t f;
	pfn_t f_link;
	const void *s1;
	const void *s2;
};
struct coctx_t
{
	void *regs[ 14 ];
	param_t *param;

	pfn_t routine;
	const void *s1;
	const void *s2;
	size_t ss_size;
	char *ss_sp;
};
#define RDI 0
#define RSI 1
#define RBX 6
#define RSP 12
#define RIP 13


param_t *b = NULL; 
coctx_t *o = 0;
coctx_t *n = 0;

extern "C"
{
	extern void start32( param_t *) asm("start32");
	extern void swapctx( coctx_t *,coctx_t* ) asm("swapctx");
};
int test( const char *s,const char *s2 )
{
	int a = 0;
	int c = 0;

	for(int i=0;i<10;i++)
	{
		printf("in routine %d [%s][%s]\n",i,s,s2);
		swapctx( n,o );
	}
	return 0;
}

typedef int (*pfn_c_t)( const char *,const char * );

static int makec( coctx_t *ctx,pfn_c_t pfn,const void *s,const void *s1 )
{
	char *stack = ctx->ss_sp;
	*stack = 0;

	char *sp = stack + ctx->ss_size - 1;
	sp = (char*)( ( (unsigned long)sp & 0xfffffffffffffff0L ) - 8);

	ctx->routine = pfn;
	ctx->s1 = s;
	ctx->s2 = s1;

	ctx->param = (param_t*)sp;
	ctx->param->f = test;	
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
static int initc( coctx_t *ctx )
{
	memset( ctx,0,sizeof(*ctx));
	return 0;
}
int main(int argc,char *argv[])
{
	int stack_size = 2048;
	char *stack = (char*)malloc( stack_size );

	//swapcontext
	o = (coctx_t*)malloc( sizeof(coctx_t) );
	n = (coctx_t*)malloc( sizeof(coctx_t) );

	initc( o );
	initc( n );

	n->ss_sp = stack;
	n->ss_size = stack_size;

	makec( n,test,"sunny_param","sunny_param2" );

	b = o->param;

	for(int i=0;i<10;i++)
	{
		printf("in main\n");
		swapctx( o,n );
	}
	printf("end\n");
	exit(0);
	return 0;
}

