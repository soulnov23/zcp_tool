#include "co_routine.h"
#include "co_routine_inner.h"
#include "co_aio_utils.h"

#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/epoll.h>

#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <libaio.h>
#include <errno.h>

#include <vector>
#include <queue>

using namespace std;

struct _iocb_ext
{
	long long   __pad3;
	unsigned    flags;
	unsigned    resfd;
};  /* result code is the amount read or -'ve errno */
static inline void _io_set_eventfd(struct iocb *iocb, int eventfd)
{
	struct _iocb_ext *p = (struct _iocb_ext *)&iocb->u.c.__pad3;
	p->flags |= (1 << 0) /* IOCB_FLAG_RESFD */;
	p->resfd = eventfd;
}

#ifndef __NR_eventfd
#define __NR_eventfd 284
#endif
#ifndef EFD_NONBLOCK
#define EFD_NONBLOCK 00004000
#endif


static inline unsigned long long now_us()
{
	struct timeval now;
	gettimeofday( &now, 0 );
	unsigned long long ret = now.tv_sec;
	ret *= 1000 * 1000;
	ret += now.tv_usec ;
	return ret;
}
class clsCoAio
{
	enum
	{
		kMaxDelayUS = 1000,
		kBatchLimit = 100,

		kAlignOff = 4096,
		kNumEventPerThread = 256,
	};
public:
	struct cb_t 
	{
		iocb io; // io.aio_lio_opcode IO_CMD_( PREAD | PWRITE | FSYNC | FDSYNC )
		long ret_code;
		unsigned long complete_bytes;
		
		stCoRoutine_t *self;
		char pendding:1; 
	};
	struct stStat
	{
		unsigned long long num_read_eventfd;

		unsigned long long align_pread_used;
		long long align_pread_cnt;

		unsigned long long submit_queue_size;
		long long submit_queue_cnt;

		stStat()
		{
			memset( this,0,sizeof(*this) );
		}
		stStat & operator=( const stStat & o )
		{
			memcpy( this,&o,sizeof(*this) );
			return *this;
		}
		void add( const stStat & o )
		{
			num_read_eventfd += o.num_read_eventfd;

			align_pread_used += o.align_pread_used;
			align_pread_cnt += o.align_pread_cnt;

			submit_queue_size += o.submit_queue_size;
			submit_queue_cnt += o.submit_queue_cnt;
		}

	};
private:
	io_context_t ctx_;
	vector< struct io_event > events_;
	int eventfd_;

	vector< iocb * > submit_queue_;
	vector< cb_t * > cb_queue_;
	unsigned long long first_submit_time_;
	stCoCond_t *submit_signal_;
	stStat stat_;
	int error_;

public:
	explicit clsCoAio( int event_cnt = kNumEventPerThread ) : 
			eventfd_(-1),
			first_submit_time_(0),
			submit_signal_(0),
			error_(0)
	{
		//io_setup
		
		memset( &ctx_,0,sizeof(ctx_) );
		events_.resize( event_cnt );
		memset( &events_[0],0,sizeof(struct io_event) * events_.size() );
		error_ = io_setup( events_.size(),&ctx_ ); // --  /proc/sys/fs/aio-max-nr 

		//eventfd

		eventfd_ = syscall( __NR_eventfd,0,EFD_NONBLOCK ); 
		fcntl( eventfd_,F_SETFL,O_NONBLOCK | fcntl( eventfd_,F_GETFD ) );
		struct stCoEvent_t *coev = co_alloc_event(eventfd_);
		co_add_event( coev, aio_event_cb, this , EPOLLIN, -1);

	}
	~clsCoAio()
	{}
	void start_tick_co()
	{
		if( submit_signal_ ) return;
		submit_signal_ = co_cond_alloc();
		stCoRoutine_t *co = 0;
		co_create(&co,0,tick_co,this );
		co_resume( co );
	}
	int error() const
	{
		return error_;
	}
	static int tick_func( void *arg )
	{
		clsCoAio *self = (clsCoAio*)arg;

		if( self->submit_signal_ ) return 0; 
		self->do_submit();
		return 0;
	}
	const stStat & Stat() const
	{
		return stat_;
	}

private:
	static void *aio_event_cb(int fd, int revent, void* args)
	{
		clsCoAio *self = ( clsCoAio* ) args;
		self->event_proc();
		return 0;
		
	}
	static void *tick_co( void *arg )
	{
		co_enable_hook_sys();
		clsCoAio *self = (clsCoAio*)arg;
		for(;;)
		{
			if( self->submit_queue_.empty() )
			{
				co_cond_timedwait( self->submit_signal_,0 );
				continue;
			}
			self->do_submit();
		}

		return 0;
	}
public:
	static ssize_t co_pread(int fd, void *buf, size_t count, off_t offset,clsCoAio *o)
	{
		if( o && o->error_ ) return o->error_;
	
		Pos raw( offset,count );
		Pos req = Pos::GetAlignPos( raw,kAlignOff );
	
		void *ptr = 0;
		posix_memalign( &ptr,kAlignOff,req.len );
		
		ssize_t ret = -1;
		if( o )
		{
			unsigned long long n = now_us();
			ret = o->align_pread(fd, ptr, req.len, req.off);
			n = now_us() - n;

			o->stat_.align_pread_used += n;
			o->stat_.align_pread_cnt++;
			//TODO: clear 
			
		}
		else
		{
			ret = pread( fd, ptr, req.len, req.off );
		}

		if( ret < 0 )
		{
			free( ptr );
			return ret;
		}
		Pos rsp( req.off,ret );
		Pos pos = Pos::GetIntersesion( raw,rsp );

		memcpy( buf,(char*)ptr + ( pos.off - rsp.off ),pos.len );
		free( ptr );
		return pos.len;
	}
private:
	void event_proc()
	{
		uint64_t evcnt = 0;
		read( eventfd_, &evcnt, sizeof(evcnt) );
		stat_.num_read_eventfd++;

		while( evcnt > 0 )
		{
			int fetch = std::min( evcnt, (uint64_t)events_.size());
			evcnt -= fetch;
			int num_ioevent = io_getevents( ctx_, 1, fetch, &events_[0], NULL );
	
			for( int i = 0;i < num_ioevent;i++)
			{
				struct io_event *ev = &events_[i];
				struct cb_t *cb = (struct cb_t*)ev->data;
	

				if( IO_CMD_PREAD == cb->io.aio_lio_opcode ||
					IO_CMD_PWRITE == cb->io.aio_lio_opcode	)
				{
					cb->ret_code = (long)ev->res2;
					cb->complete_bytes = ev->res;
					cb->pendding = 0;
				}
				co_resume( cb->self );
			}
		}
	}
	void do_submit()
	{
		if( submit_queue_.empty() )
		{
			return;
		}

		stat_.submit_queue_size += submit_queue_.size();
		stat_.submit_queue_cnt++;
		
		int ret = io_submit( ctx_,submit_queue_.size(), &submit_queue_[0] );
		//printf("io_submit %d\n",ret );
		if( ret < 1 )
		{
			printf("submit error %d\n",ret );
			assert( false );
		}
		submit_queue_.clear();

		vector< cb_t * > v;
	    cb_queue_.swap(v);

		for (size_t i = (size_t)std::max( 0,ret ); i < v.size(); i++)
		{
			v[i]->ret_code = ( ret > 0 ? ret : -1 ); 
			v[i]->pendding = 0;
			co_resume( v[i]->self );
		}
	}

	void submit_wait(cb_t &cb,struct iocb *p)
	{
		cb.pendding = 1;
		cb.self = co_self();
		p->data = &cb;
		_io_set_eventfd( p,eventfd_ );

		unsigned long long now = now_us();
		if( submit_queue_.empty() )
		{
			first_submit_time_ = now;
		}
		submit_queue_.push_back( p );
		cb_queue_.push_back( &cb );

		if (submit_queue_.size() > 1 && now - first_submit_time_ > kMaxDelayUS )
		{
			do_submit();
		}
		else if (submit_queue_.size() >= kBatchLimit )
		{
			do_submit();
		}
		else
		{
			if( submit_signal_ ) co_cond_signal( submit_signal_ );
		}
		if( cb.pendding ) co_yield_ct();
	}

public:
	static int co_fsync(int fildes,clsCoAio *aio )
	{
		struct cb_t cb = { 0 } ;
		io_prep_fsync( &cb.io, fildes );
		aio->submit_wait( cb,&cb.io );
		return 0;

	}
	static ssize_t co_pwrite(int fildes, const void *buf, size_t nbyte, off_t offset,clsCoAio *aio )
	{
		struct cb_t cb = { 0 } ;
		cb.ret_code = -1;
		cb.complete_bytes = 0;

		io_prep_pwrite( &cb.io, fildes, (void*)buf, nbyte, offset );
		aio->submit_wait( cb,&cb.io );

		if( cb.ret_code ) return cb.ret_code;
		return cb.complete_bytes;


	}
	inline ssize_t align_pread( int fildes, void *buf, size_t nbyte, off_t offset )
	{
		struct cb_t cb = { 0 } ;
		cb.ret_code = -1;
		cb.complete_bytes = 0;

		io_prep_pread ( &cb.io, fildes, buf, nbyte, offset );
		submit_wait( cb,&cb.io );

		if( cb.ret_code ) return cb.ret_code;
		return cb.complete_bytes;

	}


};
static __thread clsCoAio *o = 0;
static bool s_force_pread2 = false;
void co_set_force_pread2( bool b )
{
	s_force_pread2 = b;
}
ssize_t co_pread2(int fd, void *buf, size_t count, off_t offset );
ssize_t co_pread(int fd, void *buf, size_t count, off_t offset )
{
	if( s_force_pread2 && co_is_enable_sys_hook() ) 
	{
		return co_pread2( fd,buf,count,offset );
	}
	return clsCoAio::co_pread( fd, buf, count, offset,
								co_is_enable_sys_hook() ? o : 0 );
}
ssize_t co_pwrite(int fd, void *buf, size_t count, off_t offset )
{
	return clsCoAio::co_pwrite( fd,buf,count,offset,
								co_is_enable_sys_hook() ? o : 0 );
}
int co_fsync(int fildes)
{
	return clsCoAio::co_fsync( fildes,o );
}
int co_aio_init_ct()
{
	if ( !o ) 
	{
		o = new clsCoAio();
		o->start_tick_co();
	}
	return o->error();
}
int co_aio_tick()
{
	if( o ) clsCoAio::tick_func(o);
	return 0;
}

//----------------------------------------------------------------
class clsCoPreadThr;
struct stCoPread2_t
{
	int notify_efd;

	ssize_t ret;

	int fd;
	void *buf;
	size_t count;
	off_t offset;

	clsCoPreadThr *svr;
	int write_evfd_ret;
	const char *status;

	volatile stCoPread2_t *pHeadNext;
};
class clsCoPreadThr;
struct stCoPreadJob_t
{
	clsCoPreadThr *thr;
	stCoRoutine_t *co;
	stCoPread2_t *job;
};
static int SetNonBlock(int iSock)
{
    int iFlags;

    iFlags = fcntl(iSock, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(iSock, F_SETFL, iFlags);
    return ret;
}
class clsCoPreadThr
{
	clsLinkedList_mt<stCoPread2_t> *input_;
	queue<stCoPread2_t*> pendding_;
	int eventfd_;
	stack< stCoPreadJob_t * > idles_;
public:	
	pthread_t tid_;
	int no_;
	char tname_[64];
public:
	clsCoPreadThr()
	{
        tid_ = -1;
        no_ = -1;
		input_ = new clsLinkedList_mt<stCoPread2_t>();
		eventfd_ = syscall( __NR_eventfd,0,EFD_NONBLOCK ); 
        if (eventfd_ == -1)
        {
            assert(false);
        }
		int ret = SetNonBlock( eventfd_ );
        if (ret != 0)
        {
            co_log_err("%s:%d setnonblock ret %d errno %d", __func__, __LINE__, ret, errno);
        }
	}
    ~clsCoPreadThr()
    {
        delete input_;
    }
	bool push( stCoPread2_t * p )
	{
		volatile stCoPread2_t *a = (volatile stCoPread2_t*)p;
		bool b = input_->push_front( a,10 );
		//printf("input_->push_front %d\n",b );
		if( !b ) return b;

		uint64_t c = 1;
		write( eventfd_,&c,sizeof(c) );
		//printf("write eventfd_ %d\n",ret );

		return true;
	}

	static void *eventfd_cb( int fd, int revent, void* args)
	{
		((clsCoPreadThr*)args)->OnEvent();
		return 0;
	}
	static int tick_func( void *arg )
	{
		((clsCoPreadThr*)arg)->OnEvent();
		return 0;
	}
	void OnEvent()
	{
		uint64_t c = 0;
		read( eventfd_,&c,sizeof(c) );
		//if( c ) printf("c %llu\n",c);
		volatile stCoPread2_t *lp = input_->take_all();
		while( lp )
		{
			volatile stCoPread2_t *next = lp->pHeadNext;
			if( idles_.empty() )
			{
				lp->status = "pendding";
				pendding_.push( (stCoPread2_t*)lp );
				//printf("pendding_.size %zu\n",pendding_.size());
			}
			else
			{
				//printf("resume\n");
				lp->status = "doing";
				stCoPreadJob_t *worker = idles_.top();
				idles_.pop();
				worker->job = (stCoPread2_t *)lp;
				co_resume( worker->co );
			}
			lp = next;
		}
		
	}
	void co_run( stCoPreadJob_t *job )
	{
		co_enable_hook_sys();
		for(;;)
		{
			if( !job->job )
			{
				if( !pendding_.empty() )
				{
					job->job = pendding_.front();
					pendding_.pop();
					job->job->status = "doing";
				}
			}
			if( !job->job )
			{
				idles_.push( job );
				co_yield_ct();
				continue;
			}
			stCoPread2_t *p = job->job;
			job->job = 0;

			//printf("before_co_pread\n");
			p->status = "before_co_pread";
			p->ret = co_pread( p->fd, p->buf, p->count, p->offset );
			p->status = "after_co_pread";
			//printf("after_co_pread\n");

			uint64_t c = 1;
			write( p->notify_efd,&c,sizeof(c) );
			//p->status = "after_write_rsp";
			//p->write_evfd_ret = ret;
			//printf("rsp %d fd %d\n",ret,p->notify_efd );

		}
	}
	static void * co_func( void * a )
	{
		stCoPreadJob_t *job = (stCoPreadJob_t*)a;
		job->thr->co_run( job );
		return 0;
	}

	void run()
	{
		int co_cnt = 1024;
		o = new clsCoAio( co_cnt );
		o->start_tick_co();
		for(int i = 0; i < co_cnt; i++ )
		{
			stCoPreadJob_t *job = (stCoPreadJob_t*)calloc( 1,sizeof(stCoPreadJob_t) );
			job->thr = this;

			stCoRoutineAttr_t attr = { 0 };
			attr.stack_size = 16 * 1024;
			attr.no_protect_stack = 1;
			co_create( &job->co,&attr,co_func,job );

			co_resume( job->co );
		}
		struct stCoEvent_t *coev = co_alloc_event(eventfd_);
		co_add_event( coev, eventfd_cb, this , EPOLLIN, -1);

		co_eventloop( co_get_epoll_ct(),tick_func,this );
	}
};
#include <sys/prctl.h>
static void *co_pread_thr( void * a )
{
	clsCoPreadThr *self = (clsCoPreadThr*)a;
	snprintf( self->tname_,sizeof(self->tname_),"co_pread_%d",self->no_ );
	prctl(PR_SET_NAME,self->tname_);
	self->run();
	return 0;
}
static vector<clsCoPreadThr*> g_co_pread_thr;
void co_init_pread2()
{
	int thread_cnt = 6;
	for(int i = 0; i < thread_cnt; i++)
	{
		clsCoPreadThr *p = new clsCoPreadThr();
		g_co_pread_thr.push_back( p );
		p->no_ = i;
		pthread_create( &p->tid_,0,co_pread_thr,p);
	}
}
class clsCoPreadEventFds
{
	vector<int> eventfds_;
public:
	class auto_free
	{
		int fd_;
		clsCoPreadEventFds *this_;
	public:
		auto_free( int fd ,clsCoPreadEventFds *p ) :fd_(fd),this_(p) {}
		~auto_free() { this_->Free( fd_ ); }
	};
	clsCoPreadEventFds()
	{
		for(int i=0;i<1;i++)
		{
			int fd = syscall( __NR_eventfd,0,EFD_NONBLOCK ); 
			if( -1 == fd ) break;
			eventfds_.push_back( fd );
		}
	}
	int Alloc()
	{
		int fd = -1;
		if( !eventfds_.empty() )
		{
			fd = eventfds_.back();
			eventfds_.resize( eventfds_.size() - 1 );
		}
		if( -1 == fd )
		{
			fd = syscall( __NR_eventfd,0,EFD_NONBLOCK ); 
		}
		return fd;
	}
	void Free( int fd )
	{
		if( -1 != fd ) eventfds_.push_back( fd );
	}
};
static __thread clsCoPreadEventFds *s_eventfds = 0;

ssize_t co_pread2(int fd, void *buf, size_t count, off_t offset )
{
	if( !s_eventfds ) s_eventfds = new clsCoPreadEventFds();
	int notify_efd = s_eventfds->Alloc();
	if( -1 == notify_efd ) return -1;
	clsCoPreadEventFds::auto_free af( notify_efd,s_eventfds );


	//clsCoPreadThr *svr = g_co_pread_thr[ pthread_self() % g_co_pread_thr.size() ];
	clsCoPreadThr *svr = g_co_pread_thr[ rand() % g_co_pread_thr.size() ];
	stCoPread2_t a = { notify_efd,-1,fd,buf,count,offset,svr,-1,"init" };
	bool b = svr->push( &a ); 
	if( !b )
	{
		errno = EAGAIN;
		return -1;
	}

	uint64_t ev = 0;

	struct pollfd pf = { a.notify_efd,POLLIN };
	int ret = poll( &pf,1,-1 );
    if (ret > 0)
    {
	    ret = read( a.notify_efd, &ev, sizeof(ev) );
        if (ret <= 0)
        {
            co_log_err("%s:%d read ret %d", __func__, __LINE__, ret);
        }
    }


	return a.ret;
}



//gzrd_Lib_CPP_Version_ID--start
#ifndef GZRD_SVN_ATTR
#define GZRD_SVN_ATTR "0"
#endif
static char gzrd_Lib_CPP_Version_ID[] __attribute__((used))="$HeadURL$ $Id$ " GZRD_SVN_ATTR "__file__";
// gzrd_Lib_CPP_Version_ID--end

