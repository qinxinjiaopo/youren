#include "YR_Thread.h"

namespace youren{

YR_ThreadControl::YR_ThreadControl()
    :_thread(pthread_self())
{

}

YR_ThreadControl::YR_ThreadControl(pthread_t thread)
    :_thread(thread)
{

}

void YR_ThreadControl::join()
{
    if(pthread_self() == _thread)
    {
        throw YR_ThreadControl_Exception("[YR_ThreadControl::join] can't be called in the same thread");
    }

    void *ignore = 0;
    int rc = pthread_join(_thread, &ignore);
    if(rc != 0)
    {
        throw YR_ThreadControl_Exception("[YR_ThreadControl::join] pthread_join error",rc);
    }
}

void YR_ThreadControl::detach()
{
    if(pthread_self() == _thread)
        throw YR_ThreadControl_Exception("[YR_ThreadControl::join] can't be called in the same thread");
    int rc = pthread_detach(_thread);
    if(rc != 0)
    {
        throw YR_ThreadControl_Exception("[YR_ThreadControl::join] pthread_join error",rc);
    }
}

pthread_t YR_ThreadControl::id() const
{
    return _thread;
}


void YR_ThreadControl::sleep(long millsecond) 
{
    struct timespec ts;
    ts.tv_sec = millsecond / 1000;
    ts.tv_nsec = (millsecond % 1000)*1000000;
    nanosleep(&ts, 0);
}

void YR_ThreadControl::yield()
{
    sched_yield();
}

YR_Thread::YR_Thread()
    :_running(false), _tid(-1)
{

}

void YR_Thread::threadEntry(YR_Thread* pThread)
{
    pThread->_running = true;
    
    {
        YR_ThreadLock::Lock sync(pThread->_lock);
        pThread->_lock.notifyAll();
    }

    try
    {
        pThread->run();
    }
    catch(...)
    {
        pThread->_running = false;
        throw;
    }
    pThread->_running =false;
}

YR_ThreadControl YR_Thread::start()
{
    YR_ThreadLock::Lock sync(_lock);

    if(_running)
    {
        throw YR_ThreadCond_Exception("[YR_Thread::start] thread has start");
    }

    int ret = pthread_create(&_tid, 0, 
                             (void*(*)(void *))&threadEntry,
                             (void*)this);
    if(ret != 0)
    {
        throw YR_ThreadControl_Exception("[YR_Thread::start] thread start error");
    }
    _lock.wait();

    return YR_ThreadControl(_tid);
}


YR_ThreadControl YR_Thread::getThreadControl() const
{
    return YR_ThreadControl(_tid);
}

bool YR_Thread::isAlive() const 
{
    return _running;
}


}
