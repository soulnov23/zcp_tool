#pragma once
#include <functional>
#include <mutex>
#include "co_routine.h"
struct stCoEpoll_t;
stCoRoutine_t* co_create(const stCoRoutineAttr_t* attr,
                         std::function<void()> f);       // lambda
stCoRoutine_t* co_create(std::function<void()> f);       // lambda
int co_add_background_routine(std::function<void()> f);  // lambda func
void co_eventloop(stCoEpoll_t* ctx, std::function<int()> pfn);
