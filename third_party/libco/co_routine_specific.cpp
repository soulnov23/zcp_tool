#include "co_routine_specific.h"
static CoRoutineGetSpecificFunc_t g_pfnCoRoutineGetSpecific = NULL;
static CoRoutineSetSpecificFunc_t g_pfnCoRoutineSetSpecific = NULL;

void CoRoutineSetSpecificCallback( CoRoutineGetSpecificFunc_t pfnGet,CoRoutineSetSpecificFunc_t pfnSet )
{
    g_pfnCoRoutineGetSpecific = pfnGet;
    g_pfnCoRoutineSetSpecific = pfnSet;
}

void *CoRoutineGetSpecific(pthread_key_t key)
{
    if( g_pfnCoRoutineGetSpecific )
    {
        return g_pfnCoRoutineGetSpecific( key );
    }
    return pthread_getspecific( key );
}

int CoRoutineSetSpecific(pthread_key_t key,const void *value)
{
    if( g_pfnCoRoutineSetSpecific )
    {
        return g_pfnCoRoutineSetSpecific( key,value );
    }
    return pthread_setspecific( key, value ); 
}


//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

