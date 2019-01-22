#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "co_routine.h"
void* RoutineFunc(void* args) 
{
	int * pi = (int*)args;
	printf("Routine %d\n", *pi);
	return NULL;
}
int main(int argc, char* argv[]) 
{
	int a[10];
	stCoRoutine_t* co[10];
	for (int i = 0; i < 10; i++) 
	{
		a[i] = i;
		co_create(&co[i], NULL, RoutineFunc, &a[i]);
		co_resume(co[i]);
	}
	for (int i = 0; i < 10; i++) 
	{
		co_release(co[i]);
	}
	return 0;
}
