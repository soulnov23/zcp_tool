#pragma once
#include "co_routine.h"
#include <functional>
#include <mutex>
struct stCoEpoll_t;

stCoRoutine_t* co_create(const stCoRoutineAttr_t* attr,
                         std::function<void()> f);  // lambda
stCoRoutine_t* co_create(std::function<void()> f);  // lambda

void co_create(stCoRoutine_t** ppCo, const stCoRoutineAttr_t* attr,
               std::function<void()> f);

int co_add_background_routine(std::function<void()> f);  // lambda func
void co_eventloop(stCoEpoll_t* ctx, std::function<int()> pfn);
void co_eventloop(std::function<int()> pfn);
void co_eventloop();
