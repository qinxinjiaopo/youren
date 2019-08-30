#pragma once
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include "../ex/YR_Exception.h"
#include "YR_Monitor.h"

namespace youren {
////////////////////////////////////////////////////////////////////////////////////
//
//线程控制异常类
struct YR_ThreadControl_Exception : YR_Exception
{
    YR_ThreadControl_Exception(const string& buffer) : YR_Exception(buffer){}
    YR_ThreadControl_Exception(const string& buffer, int err) : YR_Exception(buffer, err){}
    ~YR_ThreadControl_Exception() throw() {}
};

//线程控制类
class YR_ThreadControl
{
public:
    YR_ThreadControl();

    explicit YR_ThreadControl(pthread_t);

    //等待线程结束，不能再当前线程上调用
    void join();

    //detach ,不能再当前线程上调用
    void detach();

    //获取当前线程id
    pthread_t id() const;

    //休息时间
    static void sleep(long millsecond);

    //交出当前线程控制权
    static void yield();
private:
    pthread_t _thread;
};

class YR_Runable
{
public:
    virtual ~YR_Runable(){}
    virtual void run() = 0;
};

//线程基类, 所有自定义线程都继承该类，同时实现run接口即可
//可以通过YR_ThreadControl管理线程
class YR_Thread : YR_Runable
{
public:
    YR_Thread();
    virtual ~YR_Thread() {}

    //线程运行
    YR_ThreadControl start();

    //获取线程控制类
    YR_ThreadControl getThreadControl() const;

    //线程是否存活
    bool isAlive() const;

    //获取线程id
    pthread_t id() { return _tid; }
protected:
    //线程入口，静态函数
    static void threadEntry(YR_Thread* pThread);

    //运行
    virtual void run() = 0;

protected:
    bool _running;
    pthread_t _tid;
    YR_ThreadLock _lock;
private:

};
/////////////////////////////////////////////////////////////////////////////////////
}
