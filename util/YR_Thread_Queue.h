#pragma once

#include <deque>
#include <vector>
#include <cassert>
#include "YR_Monitor.h"

//线程队列类
namespace youren{

//线程安全队列
template <class T, class D = deque<T> >
class YR_Thread_Queue : protected YR_ThreadLock
{
public:
    typedef D queue_type;
public:
    YR_Thread_Queue():_size(0) {}
    ~YR_Thread_Queue() {}

public:
    //从头部获取数据，没有数据则等待
    //millsecond 阻塞等待时间，0表示不阻塞，-1永久阻塞
    //true 获取数据，false 无数据
    bool pop_front(T& t, size_t millsecond = 0);

    //通知在队列等待的线程都醒过来
    void notifyT();

    //放数据到队列后端
    void push_back(const T& t);

    //放数据到队列后端
    void push_back(const queue_type& qt);

    //放数据到队列前端
    void push_front(const T& t);

    //放数据到队列前端
    void push_front(const queue_type& qt);

    //等到有数据才交换
    //millsecond 阻塞时间
    bool swap(queue_type &q, size_t millsecond = 0);

    //队列大小
    size_t size() const;

    //清空队列
    void clear();

    //数据是否为空
    bool empty() const;

protected:
    //队列
    queue_type _queue;
    //队列长度
    size_t _size;
    
private:

};

template <class T, class D>
bool YR_Thread_Queue<T, D>::pop_front(T& t, size_t millsecond)
{
    Lock lock(*this);

    if(_queue.empty())
    {
        if(millsecond == 0)
        {
            return false;
        }
        if(millsecond == (size_t)-1)
        {
            wait();
        }
        else
        {
            //超时了
            if(!timeWait(millsecond))
            {
                return false;
            }
        }
    }

    if(_queue.empty())
    {
        return false;
    }
    t = _queue.front();
    _queue.pop_front();
    assert(_size >0);
    --_size;

    return true;
}
template<typename T, typename D> void YR_Thread_Queue<T, D>::notifyT()
{
    Lock lock(*this);
    notifyAll();
}
template<typename T, typename D> void YR_Thread_Queue<T, D>::push_back(const T& t)
{
    Lock lock(*this);

    notify();

    _queue.push_back(t);
    ++_size;
}
template<typename T, typename D> void YR_Thread_Queue<T, D>::push_back(const queue_type &qt)
{
    Lock lock(*this);

    typename queue_type::const_iterator it = qt.begin();
    typename queue_type::const_iterator itEnd = qt.end();
    while(it != itEnd)
    {
        _queue.push_back(*it);
        ++it;
        ++_size;
        notify();
    }
}
template<typename T, typename D> void YR_Thread_Queue<T, D>::push_front(const T& t)
{
    Lock lock(*this);

    notify();

    _queue.push_front(t);

    ++_size;
}
template<typename T, typename D> void YR_Thread_Queue<T, D>::push_front(const queue_type &qt)
{
    Lock lock(*this);

    typename queue_type::const_iterator it = qt.begin();
    typename queue_type::const_iterator itEnd = qt.end();
    while(it != itEnd)
    {
        _queue.push_front(*it);
        ++it;
        ++_size;

        notify();
    }
}
template<typename T, typename D> bool YR_Thread_Queue<T, D>::swap(queue_type &q, size_t millsecond)
{
    Lock lock(*this);

    if (_queue.empty())
    {
        if(millsecond == 0)
        {
            return false;
        }
        if(millsecond == (size_t)-1)
        {
            wait();
        }
        else
        {
            //超时了
            if(!timeWait(millsecond))
            {
                return false;
            }
        }
    }

    if (_queue.empty())
    {
        return false;
    }

    q.swap(_queue);
    //_size = q.size();
    _size = _queue.size();

    return true;
}

template<typename T, typename D> size_t YR_Thread_Queue<T, D>::size() const
{
    Lock lock(*this);
    //return _queue.size();
    return _size;
}

template<typename T, typename D> void YR_Thread_Queue<T, D>::clear()
{
    Lock lock(*this);
    _queue.clear();
    _size = 0;
}

template<typename T, typename D> bool YR_Thread_Queue<T, D>::empty() const
{
    Lock lock(*this);
    return _queue.empty();
}

}
