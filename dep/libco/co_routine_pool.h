#include "co_routine.h"
#include <stack>
typedef void (*PfnWorkerCallBack)(int workeridx, void* args);
class clsRoutinePool;
struct stWorker_t 
{
	stWorker_t() 
	{
		has_job = false;
		co = NULL;
		pfn = NULL;
		args = NULL;
		pool = NULL;
		idx = 0;
	}
	void Clear() 
	{
		pfn = NULL;
		args = NULL;
		has_job = false;
	}
	void InitJob(PfnWorkerCallBack p, void* a) 
	{
		pfn = p;
		args = a;
		has_job = true;
	}
	int idx;
	bool has_job;
	stCoRoutine_t* co;
	PfnWorkerCallBack pfn;
	void* args;
	clsRoutinePool* pool;
};

class clsRoutinePool
{
	public:
		clsRoutinePool(int worker_cnt, stCoRoutineAttr_t* attr);
		static void* RoutineFunc(void* args);
		int Run(PfnWorkerCallBack func, void* args);
		void WorkerRun(stWorker_t* worker);
		
	private:
		std::stack<stWorker_t*> worker_pool;
		int max_worker_cnt;
};


