#pragma once

#include "YR_Thread.h"
#include "YR_Thread_Queue.h"
#include "YR_Monitor.h"
#include <functional>

#include <vector>
#include <set>
#include <iostream>

using namespace std;
namespace youren{
/////////////////////////////////////////////////////////////////////////////////////////////
//线程池类
//
//线程异常
struct YR_ThreadPool_Exception : public YR_Exception
{
    YR_ThreadPool_Exception(const string& buffer) :YR_Exception(buffer){}
    YR_ThreadPool_Exception(const string& buffer, int err) : YR_Exception(buffer, err){  }
    ~YR_ThreadPool_Exception() throw() {}
};

//通用线程池类
class YR_ThreadPool : public YR_ThreadLock
{
public:
    YR_ThreadPool();
    ~YR_ThreadPool();

    //初始化 num 个工作线程
    void init(size_t num);

    //获取线程个数
    size_t getThreadNum() { Lock lock(*this); return _jobthread.size(); }

    //获取线程池的任务数
    size_t getJobNum() { return _jobqueue.size(); }

    //停止所有线程
    void stop();

    //启动所有线程
    void start();

    //启动所有线程，并初始化对象
    void start(std::function<void()> tf)
    {
        for(size_t i = 0; i< _jobthread.size(); i++)
        {
            _startqueue.push_back(std::move(tf));
        }
        start();
    }

    //添加对象到线程池执行，该函数马上返回，线程池的执行对象
    void exec(std::function<void()> tf)
    {
        _jobqueue.push_back(std::move(tf));
    }

    //等待所有工作全部结束（队列无任务，无空闲线程）
    bool waitForAllDone(int millsecond = -1);

public:
    //线程数据基类，所有线程的私有数据继承于该类
    class ThreadData
    {
    public:
        ThreadData(){}
        virtual ~ThreadData(){}

        //生成数据
        template<class T>
        static T* makeThreadData()
        {
            return new T;
        }
    };

    //设置线程数据
    static void setThreadData(ThreadData *p);

    //获取线程数据
    static ThreadData* getThreadData();

    //设置线程数据，key需要自己维护
    static void setThreadData(pthread_key_t pkey, ThreadData *p);

    //获取线程数据，key需要自己维护
    static ThreadData* getThreadData(pthread_key_t pkey);
    
protected:
    //释放资源,用于pthread_create的第二个参数
    static void destructor(void *p);

    //初始化key,方便key的创建和销毁
    class KeyInitialize
    {
    public:
        KeyInitialize(){
            int ret = pthread_key_create(&YR_ThreadPool::g_key, YR_ThreadPool::destructor);
            if(ret != 0)
            {
                throw YR_ThreadPool_Exception("[YR_ThreadPool::KeyInitialize] pthread_key_create error", ret);
            }
        }

        ~KeyInitialize(){
            pthread_key_delete(YR_ThreadPool::g_key);
        }
    };

    //初始化key的控制
    static KeyInitialize g_key_initialize;
    
    //数据key
    static pthread_key_t g_key;
protected:
    //线程池中的工作线程
    class ThreadWorker : public YR_Thread
    {
    public:
        ThreadWorker(YR_ThreadPool *tpool)
            :_tpool(tpool),_bTerminate(false){}

        //通知线程结束
        void terminate();
    protected:
        virtual void run();
    protected:
        //线程池指针
        YR_ThreadPool *_tpool;
        //是否结束线程
        bool _bTerminate;
    };

protected:
    //清除
    void clear();

    //获取任务
    std::function<void()> get(ThreadWorker* ptw);

    //获取启动任务
    std::function<void()> get();

    //空闲一个线程
    void idle(ThreadWorker* ptw);

    //通知等待在任务队列上的工作线程醒来
    void notifyT();

    //是否处理结束
    bool finish();

    //线程退出时调用
    void exit();

    friend class ThreadWorker;
protected:
    //任务队列
    YR_Thread_Queue<std::function<void()>> _jobqueue;

    //启动任务
    YR_Thread_Queue<std::function<void()>> _startqueue;

    //工作线程
    std::vector<ThreadWorker*> _jobthread;

    //繁忙线程
    std::set<ThreadWorker*> _busthread;

    //任务队列的锁
    YR_ThreadLock _tmutex;

    //是否所有任务执行完毕
    bool _bAllDone;
private:

};
////////////////////////////////////////////////////////////////////////////////////////////
}

