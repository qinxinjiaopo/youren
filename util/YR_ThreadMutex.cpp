#include "YR_ThreadMutex.h"
#include <iostream>
#include <cstring>
#include <cassert>

namespace youren{
    
YR_ThreadMutex::YR_ThreadMutex()
{
    int rc;
    pthread_mutexattr_t attr;
    
    rc = pthread_mutexattr_init(&attr);
    assert(rc == 0);

    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    assert(rc ==0);

    rc =pthread_mutex_init(&_mutex, &attr);
    assert(rc ==0);

    rc =pthread_mutexattr_destroy(&attr);
    assert(rc ==0);

    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadMutex::YR_ThreadMutex] pthread_mutexattr_init error", rc);
    }
}

YR_ThreadMutex::~YR_ThreadMutex()
{
    int rc =0;
    rc = pthread_mutex_destroy(&_mutex);
    if(rc != 0)
    {
        cerr << "[YR_ThreadMutex::~YR_ThreadMutex] pthread_mutex_destory error: "<<string(strerror(rc)) <<endl;
    }
}

void YR_ThreadMutex::lock() const
{
    int rc = pthread_mutex_lock(&_mutex);
    if(rc != 0)
    {
        if( rc == EDEADLK )
        {
            throw YR_ThreadMutex_Exception("[YR_ThreadMutex::lock] pthread_mutex_lock dead lock error", rc);
        }
        else
        {
            throw YR_ThreadMutex_Exception("[YR_ThreadMutex::lock] pthread_mutex_lock error", rc);
        }
    }
}

bool YR_ThreadMutex::tryLock() const
{
    int rc= pthread_mutex_trylock(&_mutex);
    if(rc != 0 && rc != EBUSY)
    {
        if(rc == EDEADLK)
        {
            throw YR_ThreadMutex_Exception("[YR_ThreadMutex::tryLock] pthread_mutex_tryLock dead lock error", rc);
        }
        else
        {
            throw YR_ThreadMutex_Exception("[YR_ThreadMutex::tryLock] pthread_mutex_tryLock error", rc);
        }
    }
    return rc == 0;
}

void YR_ThreadMutex::unlock() const
{
    int rc =pthread_mutex_unlock(&_mutex);
    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadMutex::unlock] pthread_mutex_unlock error", rc);
    }
}

int YR_ThreadMutex::count() const
{
    return 0;
}

void YR_ThreadMutex::count(int c) const
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

YR_ThreadRecMutex::YR_ThreadRecMutex()
    :_count(0)
{
    int rc;

    pthread_mutexattr_t attr;
    rc = pthread_mutexattr_init(&attr);
    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::YR_ThreadRecMutex] pthread_mutexattr_init error", rc);
    }

    rc =pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::YR_ThreadRecMutex] pthread_mutexattr_settype error", rc);
    }
    rc =pthread_mutex_init(&_mutex,&attr);
    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::YR_ThreadRecMutex] pthread_mutex_init error", rc);
    }
    rc =pthread_mutexattr_destroy(&attr);
    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::YR_ThreadRecMutex] pthread_mutexattr_destroy error", rc);
    }
}

YR_ThreadRecMutex::~YR_ThreadRecMutex()
{
    while(_count)
    {
        unlock();
    }
    int rc = 0;
    rc =pthread_mutex_destroy(&_mutex);
    if(rc != 0)
    {
        cerr << "[YR_ThreadRecMutex::~YR_ThreadRecMutex] pthread_mutex_destroy error" << string(strerror(rc)) <<endl;
    }
}

int YR_ThreadRecMutex::lock() const
{
    int rc = pthread_mutex_lock(&_mutex);
    if(rc != 0)
    {
        throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::lock] pthread_mutex_lock error",rc);
    }
    if(++_count > 1)
    {
        rc = pthread_mutex_unlock(&_mutex);
        assert(rc == 0);
    }
    return rc;
}

int YR_ThreadRecMutex::unlock() const
{
   if(--_count == 0)
   {
       int rc =pthread_mutex_unlock(&_mutex);
       assert(rc == 0);
       return rc;
   }
   return 0;
}

bool YR_ThreadRecMutex::tryLock() const
{
    int rc = pthread_mutex_trylock(&_mutex);
    if(rc != 0)
    {
        if(rc != EBUSY)
        {
            throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::tryLock] pthread_mutex_trylock error",rc);
        }
    }
    else if(++_count >1)
    {
        rc = pthread_mutex_unlock(&_mutex);
        if(rc != 0)
        {
            throw YR_ThreadMutex_Exception("[YR_ThreadRecMutex::tryLock] pthread_mutex_unlock error",rc);
        }
    }
    return rc == 0;
}

bool YR_ThreadRecMutex::willUnlock() const
{
    return _count == 1;
}

int YR_ThreadRecMutex::count() const
{
    int c = _count;
    _count = 0;
    return c;
}

void YR_ThreadRecMutex::count(int c) const
{
    _count = c;
}


}
