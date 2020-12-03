#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int test( const char *s,const char *s2 )
{
	printf("[%s][%s]\n",s,s2);
	printf("sunny\n");
	return 0;
}
typedef int (*pfn_t)( const char *s,const char *s2 );
struct param64_t
{
	pfn_t f;//0
	pfn_t f_link;//8
	const char *s1;//16
	const char *s2;//24
};
extern "C"
{
	extern void swapc( param64_t *) asm("swapc");
};
int main(int argc,char *argv[])
{
	int stack_size = 1024;
	char *stack = (char*)malloc( stack_size );

	param64_t *t = (param64_t*)( (unsigned long)(stack + stack_size - sizeof( param64_t )) & 0xfffffff0 );


	t->f = test;	
	t->f_link = 0;
	t->s1 = "sunny_param";
	t->s2 = "sunny_param2";
 	swapc( t );
	printf("sunny\n");
	return 0;
}
