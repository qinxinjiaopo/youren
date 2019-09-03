#include "YR_ThreadCond.h"
#include "YR_TimeProvider.h"
#include <string.h>
#include <cassert>
#include <iostream>

namespace youren{

YR_ThreadCond::YR_ThreadCond()
{
    int rc;
    pthread_condattr_t attr;			//定义条件属性对象
    rc = pthread_condattr_init(&attr);	//1. 初始化条件属性对象
    if(rc != 0)
    {
        throw YR_ThreadCond_Exception("[YR_ThreadCond::YR_ThreadCond] pthread_condattr_init error", errno);
    }

    rc = pthread_cond_init(&_cond, &attr);	//2. 初始化条件
    if(rc != 0)
    {
        throw YR_ThreadCond_Exception("[YR_ThreadCond::YR_ThreadCond] pthread_cond_init error", errno);
    }

    rc = pthread_condattr_destroy(&attr);	//3. 销毁条件属性对象
    if(rc != 0)
    {
        throw YR_ThreadCond_Exception("[YR_ThreadCond::YR_ThreadCond] pthread_condattr_destroy error", errno);
    }
}

YR_ThreadCond::~YR_ThreadCond()
{
    int rc = 0;
    rc = pthread_cond_destroy(&_cond);
    if(rc != 0)
    {
        cerr << "[YR_ThreadCond::~YR_ThreadCond] pthread_cond_destroy error:" << string(strerror(rc)) << endl;
    }
}
void YR_ThreadCond::signal()
{
    int rc = pthread_cond_signal(&_cond);
    if(rc != 0)
    {
        throw YR_ThreadCond_Exception("[YR_ThreadCond::signal] pthread_cond_signal error", errno);
    }
}

void YR_ThreadCond::broadcast()
{
    int rc = pthread_cond_broadcast(&_cond);
    if(rc != 0)
    {
        throw YR_ThreadCond_Exception("[YR_ThreadCond::broadcast] pthread_cond_broadcast error", errno);
    }
}
timespec YR_ThreadCond::abstime( int millsecond) const
{
    struct timeval tv;
    gettimeofday(&tv, 0);		//时间操作将会被封装，如下注释
    //TC_TimeProvider::getInstance()->getNow(&tv);
    //it 精度为微秒
    int64_t it  = tv.tv_sec * (int64_t)1000000 + tv.tv_usec + (int64_t)millsecond * 1000;

    tv.tv_sec   = it / (int64_t)1000000;
    tv.tv_usec  = it % (int64_t)1000000;

    timespec ts;
    ts.tv_sec   = tv.tv_sec;
    ts.tv_nsec  = tv.tv_usec * 1000;

    return ts;
}

//注：timeval的最高精度为微秒，timespec的最高精度为纳秒
//	timeval由gettimeofday()获取系统时间
//  timespec由clock_gettime(clockid_t, struct timespec *)获取特定时间
//		CLOCK_REALTIME 统当前时间，从1970年1.1日算起
//		CLOCK_MONOTONIC 系统的启动时间，不能被设置
//		CLOCK_PROCESS_CPUTIME_ID 本进程运行时间
//		CLOCK_THREAD_CPUTIME_ID 本线程运行时间
}

