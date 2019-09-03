#pragma once
#include <stdexcept>
#include <cerrno>
#include <string>

#include "YR_Exception.h"

using namespace std;
namespace youren {
/////////////////////////////////////////////////////////////////////////////////
//
/* 锁异常 */
struct YR_Lock_Exception : YR_Exception
{
    YR_Lock_Exception(const string& buffer) : YR_Exception(buffer) {  }
    YR_Lock_Exception(const string& buffer, int err) : YR_Exception(buffer, err) {  }
    ~YR_Lock_Exception() throw(){  }

};

//构造时加锁， 析构是解锁
//其他具体锁配合使用
template <class T>
class YR_LockT
{
public:
    //构造时加锁
    YR_LockT(const T& mutex) : _mutex(mutex) { _mutex.lock(); _acquired = true;}
    //析构时解锁
    virtual ~YR_LockT() {
        if(_acquired)
        {
            _mutex.unlock();
        }
    }

    //上锁，如果已经上锁，抛出异常
    void acquire() const 
    {
        if(_acquired)
        {
            throw YR_Lock_Exception("thread has locked!");
        }
        _mutex.lock();
        _acquired = true;
    }

    //尝试上锁，成功返回true，错误返回false
    bool tryAcquired() const
    {
        _acquired = _mutex.tryLock();
        return _acquired;
    }

    //释放锁，如果没有上锁，抛出异常
    void release() const
    {
        if(!_acquired)
        {
            throw YR_Lock_Exception("thread hasn't been locked!");
        }
        _mutex.unlock();
        _acquired =false;
    }

    //是否已上锁
    bool acquired() const
    {
        return _acquired;
    }

protected:
    //构造函数 用于锁尝试操作，与YR_LockT相似
    YR_LockT(const T& mutex, bool) : _mutex(mutex)
    {
        _acquired = _mutex.tryLock();
    }
private:
    YR_LockT(const YR_LockT&);
    YR_LockT& operator=(const YR_LockT&);

protected:
    //锁对象
    const T& _mutex;
    //是否已经上锁
    mutable bool _acquired;

};

template<class T>
class YR_TryLock : public YR_LockT<T>
{
public:
    YR_TryLock(const T& mutex) : YR_LockT<T>(mutex, true){}
};

//空锁，不做任何锁动作
class YR_EmptyMutex
{
public:
    //写锁
    int lock() const { return 0; }
    //解写锁
    int unlock() const { return 0; }
    //尝试解锁
    bool trylock() const { return true; }
};

//读写锁读锁模板类
//构造加锁，析构解锁
template <class T>
class YR_RW_RLockT
{
public:
    YR_RW_RLockT(T& lock)
        :_rwLock(lock),_acquired(false)
    {

    }
    ~YR_RW_RLockT()
    {
        if(_acquired)
        {
            _rwLock.Unlock();
        }
    }
private:
    const T& _rwLock;
    //是否已经上锁
    mutable bool _acquired;

    YR_RW_RLockT(const YR_RW_RLockT&);
    YR_RW_RLockT& operator=(const YR_RW_RLockT&);
};

//读写锁写锁模板
template < class T>
class YR_RW_WLockT
{
public:
    YR_RW_WLockT(T& lock)
        : _rwLock(lock), _acquired(false)
    {}
    ~YR_RW_WLockT()
    {
        if(_acquired)
        {
            _rwLock.Unlock();
        }
    }
private:
    const T& _rwLock;
    mutable bool _acquired;

    YR_RW_WLockT(const YR_RW_WLockT&);
    YR_RW_WLockT& operator=(const YR_RW_WLockT&);
};
////////////////////////////////////////////////////////////////////////////////
}

