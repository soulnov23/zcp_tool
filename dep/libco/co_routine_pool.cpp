#include "co_routine_pool.h"
#include <assert.h>

clsRoutinePool::clsRoutinePool(int worker_cnt, stCoRoutineAttr_t* attr) 
{
	assert(worker_cnt > 0);
	max_worker_cnt = worker_cnt;
	for (int i = 0; i < worker_cnt; i++) 
	{
		stCoRoutine_t* co = NULL;
		stWorker_t* worker = new stWorker_t;
		worker->pool = this;
		worker->idx = i;
		co_create(&co, attr, clsRoutinePool::RoutineFunc, worker);
		co_resume(co);
	}
}

void* clsRoutinePool::RoutineFunc(void* args)
{
	stWorker_t* worker = (stWorker_t*)args;
	clsRoutinePool* pool = worker->pool;
	pool->WorkerRun(worker);
	return NULL;
}
void clsRoutinePool::WorkerRun(stWorker_t* worker) 
{
	//enable hook
	co_enable_hook_sys();

	worker->co = co_self();
	while (true) 
	{
		if (!worker->has_job || worker->pfn == NULL) 
		{
			worker_pool.push(worker);
			co_yield_ct();
			continue;
		}

		//do job
		worker->pfn(worker->idx, worker->args);

		//recycle co
		worker->Clear();
	}
}

//���룺�ص������Լ��������ص������޷���ֵ
//����ֵ�� 0 ����Э��ִ��
//         -1 ����ʧ�ܣ�Э�̳ؿռ䲻��
int clsRoutinePool::Run(PfnWorkerCallBack func, void* args)
{
	if (!worker_pool.empty())
	{
		stWorker_t* worker = worker_pool.top();
		worker_pool.pop();
		worker->InitJob(func, args);
		co_resume(worker->co);
		return 0;
	}
	return -1;
}
