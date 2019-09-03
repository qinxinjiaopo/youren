#pragma once
#include "YR_Exception.h"
#include <sys/time.h>

using namespace std;
namespace youren {
//////////////////////////////////////////////////////////////////////////////////////////

struct YR_ThreadCond_Exception : public YR_Exception
{
    YR_ThreadCond_Exception(const string& buffer) : YR_Exception(buffer){}
    YR_ThreadCond_Exception(const string& buffer, int err) : YR_Exception(buffer, err){}
    ~YR_ThreadCond_Exception() throw(){}
};

//线程信号条件类，所有锁可以在上面等待信号发生，和YR_ThreadMutex、YR_ThreadRecMutex配合使用
class YR_ThreadCond
{
public:
    YR_ThreadCond();
    ~YR_ThreadCond();

    //发送信号，等待在该条件的一个线程会醒
    void signal();

    //等待在该条件上的所有线程会醒
    void broadcast();

    //获取绝对等待时间
    timespec abstime(int millsecond) const;

    //无限制等待
    template<typename Mutex>
    void wait(const Mutex& mutex) const
    {
        int c = mutex.count();
        int rc = pthread_cond_wait(&_cond, &mutex._mutex);
        mutex.count(c);
        if(rc != 0)
        {
            throw YR_ThreadCond_Exception("[YR_ThreadCond::wait] pthread_cond_wait error", errno);
        }
    }

    //等待时间，false表示超时，true表示事件来了
    template <typename Mutex>
    bool timeWait(const Mutex& mutex, int millsecond) const
    {
        int c =mutex.count();
        timespec ts = abstime(millsecond);
        int rc = pthread_cond_timedwait(&_cond, &mutex._mutex, &ts);
        mutex.count(c);

        if(rc != 0)
        {
            if(rc != ETIMEDOUT)
            {
                throw YR_ThreadCond_Exception("[YR_ThreadCond::timewait] pthread_cond_timewait erorr", errno);
            }
            return false;
        }
        return true;
    }
protected:
    YR_ThreadCond(const YR_ThreadCond& );
    YR_ThreadCond& operator=(const YR_ThreadCond&);

private:
    //线程条件
    mutable pthread_cond_t _cond;
};
//////////////////////////////////////////////////////////////////////////////////////////
}
