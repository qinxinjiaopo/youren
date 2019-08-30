#pragma once
#include <string>
#include <string.h>
#include "YR_Thread.h"
#include "YR_AutoPtr.h"

#define rdtsc(low,high) \
    __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))
#define TNOW youren::YR_TimeProvider::getInstance()->getNow()
#define NTNOWS youren::YR_TimeProvider::getInstance()->getNowMs()

namespace youren {
////////////////////////////////////////////////////////////////////////////////
//
//
//提供秒级别的时间
class YR_TimeProvider : public YR_Thread, public YR_HandleBase
{
public:
    typedef YR_AutoPtr<YR_TimeProvider> YR_TimeProviderPtr;
    //获取实例
    static YR_TimeProvider* getInstance();

    YR_TimeProvider() : _terminate(false),_use_tsc(true), _cpu_cycle(0),_buf_idx(0){
        memset(_t,0, sizeof(_t));
        memset(_tsc, 0, sizeof(_tsc));

        struct  timeval tv;
        ::gettimeofday(&tv, NULL);
        _t[0] = tv;
        _t[1] = tv;
    }
    ~YR_TimeProvider();

    //获取当前时间
    time_t getNow() { return _t[_buf_idx].tv_sec; }

    //获取时间
    void getNow(timeval * tv);

    //获取ms时间
    int64_t getNowMs();

    //获取CPU主频
    float cpuMHz();

protected:
    virtual void run();
    static YR_ThreadLock g_tl;
    static YR_TimeProviderPtr g_tp;
private:
    void setTsc(timeval& tt);
    void addTimeOffset(timeval& tt, const int &idx);
protected:
    bool _terminate;
    bool _use_tsc;
private:
    float _cpu_cycle;
    volatile int _buf_idx;
    timeval _t[2];
    uint64_t _tsc[2];
};
////////////////////////////////////////////////////////////////////////////////
}
