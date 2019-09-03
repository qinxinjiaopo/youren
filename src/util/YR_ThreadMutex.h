#pragma once
#include "YR_Lock.h"

using namespace std;
namespace youren {
///////////////////////////////////////////////////////////////////////////
//线程互斥对象
struct YR_ThreadMutex_Exception : public YR_Lock_Exception
{
    YR_ThreadMutex_Exception(const string& buffer) : YR_Lock_Exception(buffer){}
    YR_ThreadMutex_Exception(const string& buffer,int err) : YR_Lock_Exception(buffer, err){  }
    ~YR_ThreadMutex_Exception() throw(){}
};
//线程锁
//不可重复加锁，通常不直接使用,和YR_ThreadLock搭配使用
class YR_ThreadMutex
{
public:
    YR_ThreadMutex(); 
    virtual ~YR_ThreadMutex();

    void lock() const;

    bool tryLock() const;

    void unlock() const;

    //加锁后调用unlock是否会解锁，给YR_Monitor使用永远返回true
    bool willUnlock() const { return true; }
protected:

    YR_ThreadMutex(const YR_ThreadMutex&);
    void operator=(const YR_ThreadMutex&);

    int count() const;
    void count(int c)const;
    friend class YR_ThreadCond;

protected:
    mutable pthread_mutex_t _mutex;
private:

};

//线程锁类，采用线程库实现
class YR_ThreadRecMutex
{
public:
    YR_ThreadRecMutex();

    ~YR_ThreadRecMutex();

    //锁，调用pthread_mutex_lock
    int lock() const;

    //解锁,调用pthread_mutex_unlock
    int unlock() const;

    //尝试锁，失败抛出异常
    bool tryLock() const;

    bool willUnlock() const;
protected:
    friend class YR_ThreadCond;

    int count() const;

    void count(int c)const;

private:
    mutable pthread_mutex_t _mutex;
    mutable int _count;
};
////////////////////////////////////////////////////////////////////////
}

