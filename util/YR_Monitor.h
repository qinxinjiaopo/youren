#pragma once
#include "YR_ThreadMutex.h"
#include "YR_ThreadCond.h"

namespace youren {
///////////////////////////////////////////////////////////////////////////////////////

//线程锁监控模板类
//通常线程锁都通过该类使用，而不是直接使用YR_ThreadMutex、YR_ThreadRecMutex
//
//该类将YR_ThreadMutex和YR_ThreadCond结合来使用

template <class T, class P>
class YR_Monitor
{
public:
    typedef YR_LockT<YR_Monitor<T,P> > Lock;
    typedef YR_TryLock<YR_Monitor<T,P> > TryLock;
    YR_Monitor() : _nnotify(0) {}
    virtual ~YR_Monitor() {}

    void lock() const
    {
        _mutex.lock();
        _nnotify = 0;
    }

    void unlock() const
    {
        notifyImpl(_nnotify);
        _mutex.unlock();
    }

    bool tryLock() const 
    {
        bool result = _mutex.tryLock();
        if(result)
        {
            _nnotify = 0;
        }
        return result;
    }

    //当前调用线程在锁上等待，直到事件通知
    void wait() const
    {
        notifyImpl(_nnotify);

        try{
            _cond.wait(_mutex);
        }catch(...)
        {
            _nnotify =0;
            throw;
        }
        _nnotify = 0;
    }

    //等待时间，当前调用线程在锁上等待，直到超时或者有事件通知
    bool timeWait(int millsecond) const
    {
        notifyImpl(_nnotify);
        bool rc;

        try
        {
            rc = _cond.timeWait(_mutex, millsecond);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
        return rc;
    }

    //通知某一个线程醒来
    void notify()
    {
        if(_nnotify != -1)
        {
            ++_nnotify;
        }
    }

    //通知等待在该锁上的所有线程醒来
    void notifyAll()
    {
        _nnotify = -1;
    }

protected:
    //通知实现, nnotify为上锁的次数
    void notifyImpl(int nnotify) const
    {
        if(nnotify != 0)
        {
            if(nnotify == -1)
            {
                _cond.broadcast();
                return;
            }
            else
            {
                while(nnotify > 0)
                {
                    _cond.signal();
                    --nnotify;
                }
            }
        }
    }
protected:
    mutable int _nnotify;
    mutable P _cond;
    T _mutex;
private:
    YR_Monitor(const YR_Monitor&);
    void operator=(const YR_Monitor&);

};

//普通线程锁
typedef YR_Monitor<YR_ThreadMutex,YR_ThreadCond> YR_ThreadLock;

//循环锁(一个线程可以加多次锁)
typedef YR_Monitor<YR_ThreadRecMutex, YR_ThreadCond> YR_ThreadRecLock;

///////////////////////////////////////////////////////////////////////////////////////
}

