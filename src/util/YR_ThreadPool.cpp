#include "YR_ThreadPool.h"
#include "YR_Common.h"

namespace youren {

YR_ThreadPool::KeyInitialize YR_ThreadPool::g_key_initialize;
pthread_key_t YR_ThreadPool::g_key;
YR_ThreadPool::YR_ThreadPool()
    :_bAllDone(true)
{
}

YR_ThreadPool::~YR_ThreadPool()
{
    stop();
    clear();
}

void YR_ThreadPool::init(size_t num)
{
    stop();
    Lock sync(*this);
    clear();
    for(size_t i = 0; i<num; i++)
    {
        _jobthread.push_back(new ThreadWorker(this));
    }
}

void YR_ThreadPool::stop()
{
    Lock sync(*this);
    auto it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        if((*it)->isAlive())
        {
            (*it)->terminate();
            (*it)->getThreadControl().join();       //线程回收
        }
        ++it;
    }
}

void YR_ThreadPool::start()
{
    Lock sync(*this);
    auto it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        (*it)->start();
        ++it;
    }
    _bAllDone = false;
}

void YR_ThreadPool::setThreadData(YR_ThreadPool::ThreadData *p)
{
    //释放原来数据
    YR_ThreadPool::ThreadData *pOld = getThreadData();
    if(pOld != NULL && pOld != p)
        delete pOld;

    int ret = pthread_setspecific(g_key, (void*)p);
    if(ret != 0)
    {
        throw YR_ThreadPool_Exception("[YR_ThreadPool::setThreadData] pthread_setspecific error", ret);
    }
}

void YR_ThreadPool::setThreadData(pthread_key_t pkey, ThreadData *p)
{
    YR_ThreadPool::ThreadData *pOld = getThreadData(pkey);
    if(pOld != NULL && pOld != p)
        delete pOld;
    int ret = pthread_setspecific(pkey, (void*)p);
    if(ret != 0)
    {
        throw YR_ThreadPool_Exception("[YR_ThreadPool::setThreadData] pthread_setspecific error", ret);
    }
}

YR_ThreadPool::ThreadData* YR_ThreadPool::getThreadData()
{
    return (ThreadData*)pthread_getspecific(g_key);
}

YR_ThreadPool::ThreadData* YR_ThreadPool::getThreadData(pthread_key_t pkey)
{
    return (ThreadData*)pthread_getspecific(pkey);
}


void YR_ThreadPool::destructor(void *p)
{
    ThreadData* ttd = (ThreadData*)p;
    delete ttd;
}

void YR_ThreadPool::clear()
{
    auto it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        delete(*it);
        ++it;
    }
    _jobthread.clear();
    _jobqueue.clear();
}

std::function<void()> YR_ThreadPool::get(ThreadWorker* ptw)
{
    std::function<void()> res;
    if(!_jobqueue.pop_front(res, 1000))
    {
        return NULL;
    }
    {
        Lock sync(_tmutex);
        _busthread.insert(ptw); 
    }
    return res;
}

std::function<void()> YR_ThreadPool::get()
{
    std::function<void()> res;
    if(!_startqueue.pop_front(res))
    {
        return NULL;
    }
    return res;
}

void YR_ThreadPool::exit()
{
    YR_ThreadPool::ThreadData *p = getThreadData();
    if(p)
    {
        delete p;
        int ret = pthread_setspecific(g_key, NULL); 
        if(ret != 0)
        {
            throw YR_ThreadPool_Exception("[YR_ThreadPool::exit] pthread_setspecific error", ret);
        }
    }
    _jobqueue.clear();
}

void YR_ThreadPool::notifyT()
{
    _jobqueue.notifyT();
}

bool YR_ThreadPool::finish()
{
    return _startqueue.empty() && _jobqueue.empty() && _busthread.empty() &&  _bAllDone;
}

void YR_ThreadPool::idle(ThreadWorker* ptw)
{
    Lock sync(_tmutex);
    _busthread.erase(ptw);

    if(_busthread.empty())
    {
        _bAllDone = true;
        _tmutex.notifyAll();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////

void YR_ThreadPool::ThreadWorker::run()
{
    //获取启动任务
    //调用初始化部分
    auto pst = _tpool->get();
    if(pst)
    {
        try{
            pst();
        }
        catch(...)
        {
        }
    }
    
    //调用处理部分
    while(!_bTerminate)
    {
        auto pfw = _tpool->get(this);
        if(pfw)
        {
            try
            {
                pfw();
            }
            catch(...)
            {

            }
            _tpool->idle(this);
        }
    }
    _tpool->exit();
}


void YR_ThreadPool::ThreadWorker::terminate()
{
    _bTerminate = true;
    _tpool->notifyT();
}

bool YR_ThreadPool::waitForAllDone(int millsecond)
{
    Lock sync(_tmutex);
start1:
    //任务队列和繁忙线程都是空的
    if (finish())
    {
        return true;
    }
    //永远等待
    if(millsecond < 0)
    {
        _tmutex.timeWait(1000);
        goto start1;
    }
    int64_t iNow= YR_Common::nowToms();
    int m       = millsecond;
start2:
    bool b = _tmutex.timeWait(millsecond);
    //完成处理了
    if(finish())
    {
        return true;
    }
    if(!b)
    {
        return false;
    }
    millsecond = max((int64_t)0, m  - (YR_Common::nowToms() - iNow));
    goto start2;

    return false;
}
}
