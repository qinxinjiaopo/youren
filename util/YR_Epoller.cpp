#include "YR_Epoller.h"
#include <unistd.h>

namespace youren{

YR_Epoller::YR_Epoller(bool bEt /*= true*/)
{
    _iEpollfd = -1;
    _pevs = NULL;
    _et = bEt;
    _max_connections = 1024;
}
YR_Epoller::~YR_Epoller()
{
    if(_pevs != NULL)
    {
        delete[] _pevs;
        _pevs = NULL;
    }
    if(_iEpollfd != -1)
    {
        close(_iEpollfd);
    }
}
void YR_Epoller::create(int max_connections)
{
    _max_connections = max_connections;
    _iEpollfd = epoll_create(_max_connections);
    delete[] _pevs;
    _pevs = new epoll_event[_max_connections + 1];
}

void YR_Epoller::ctrl(int fd, long long data, __uint32_t events, int op)
{
    struct epoll_event ev;
    ev.data.u64 = data;
    //set mode EPOLLET.
    if(_et)
    {
        ev.events = events | EPOLLET;
    }
    else
    {
        ev.events = events;
    }
    epoll_ctl(_iEpollfd, op, fd, &ev);
}

void YR_Epoller::add(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_ADD);
}

void YR_Epoller::del(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_DEL);
}

void YR_Epoller::mod(int fd, long long data, __uint32_t event)
{
    ctrl(fd, data, event, EPOLL_CTL_MOD);
}


int YR_Epoller::wait(int millseconds)
{
    //int epoll_wait(int epfd, struct epoll_event* events, int maxevents,int timeout).
    //return numbers of need handel events.
    return epoll_wait(_iEpollfd, _pevs, _max_connections+1, millseconds);
}

}
