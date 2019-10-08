#ifndef __TASK_H__
#define __TASK_H__

class task
{
public:
	task(){}
	virtual ~task(){}

	virtual void* get_msg() = 0;

	virtual int get_fd() = 0;

	virtual void send() = 0;
};

#endif