#include "YR_TimeProvider.h"

namespace youren {

YR_ThreadLock YR_TimeProvider::g_tl;
YR_TimeProvider::YR_TimeProviderPtr YR_TimeProvider::g_tp = NULL;

YR_TimeProvider* YR_TimeProvider::getInstance()
{
    if(!g_tp)
    {
        YR_ThreadLock::Lock lock(g_tl);
        if(!g_tp)
        {
            g_tp = new YR_TimeProvider();
            g_tp->start();
        }
    }
    return g_tp.get();
}

YR_TimeProvider::~YR_TimeProvider()
{
    {
        YR_ThreadLock::Lock lock(g_tl);
        _terminate = true;
        g_tl.notify();
    }
    getThreadControl().join();
}

void YR_TimeProvider::getNow(timeval * tv)
{
    int idx = _buf_idx;
    *tv = _t[idx];

    if(_cpu_cycle != 0 && _use_tsc)
    {
        addTimeOffset(*tv, idx);
    }
    else
    {
        ::gettimeofday(tv, NULL);
    }
}

int64_t YR_TimeProvider::getNowMs()
{
    struct timeval tv;
    getNow(&tv);
    return tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;
}

void YR_TimeProvider::run()
{
    while(!_terminate)
    {
        timeval& tt = _t[!_buf_idx];
        ::gettimeofday(&tt, NULL);
        setTsc(tt);
        _buf_idx = !_buf_idx;
        YR_ThreadLock::Lock lock(g_tl);
        g_tl.timeWait(800);      //修改800时，需要对应修改addTimeOffset中offset判读值
    }
}

float YR_TimeProvider::cpuMHz()
{
    if(_cpu_cycle != 0)
    {
        return 1.0/_cpu_cycle;
    }
    return 0;
}

void YR_TimeProvider::setTsc(timeval& tt)
{
    uint32_t low = 0;
    uint32_t high = 0;
    rdtsc(low, high);
    uint64_t current_tsc = ((uint64_t)high<<32) | low;

    uint64_t& last_tsc  = _tsc[!_buf_idx];
    timeval& last_tt = _t[_buf_idx];

    if(_tsc[_buf_idx] == 0 || _tsc[!_buf_idx] == 0)
    {
        _cpu_cycle = 0;
        last_tsc = current_tsc;
    }
    else
    {
        time_t sptime = (tt.tv_sec - last_tt.tv_sec)*1000*1000 + (tt.tv_usec - last_tt.tv_usec);
        _cpu_cycle = (float)sptime/(current_tsc - _tsc[_buf_idx]);
        last_tsc = current_tsc;
    }
}

void YR_TimeProvider::addTimeOffset(timeval& tt, const int &idx)
{
    uint32_t low = 0;
    uint32_t high = 0;
    rdtsc(low, high);
    uint64_t current_tsc = ((uint64_t)high<<32) | low;
    int64_t t = (int64_t)(current_tsc -_tsc[idx]);
    time_t offset = (time_t)(t*_cpu_cycle);
    if(t < -1000 || offset > 1000000)
    {
        _use_tsc =false;
        ::gettimeofday(&tt, NULL);
        return;
    }
    tt.tv_usec += offset;
    while(tt.tv_usec >= 1000000)
    {
        tt.tv_usec -= 1000000;
        tt.tv_sec++;
    }
}


}
