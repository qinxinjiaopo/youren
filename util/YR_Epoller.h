#pragma once

#include <sys/epoll.h>
#include <cassert>

namespace youren{
///////////////////////////////////////////////////////
// class of EPOLL Operator
// ////////////////////////////////////////////////////
//

//YR_Epoller default trigger is EPOLLTE
class YR_Epoller
{
public:
    //bEt default mode is ET. Get notify when status change. 
    YR_Epoller(bool bEt = true);
    ~YR_Epoller();

    //create epoll handle.
    //max_connections epoll: Maximum number of connections required for service.
    void create(int max_connections);

    //add fd to epfd.
    void add(int fd, long long data, __uint32_t event);
    //del fd from epfd.
    void del(int fd, long long data, __uint32_t event);
    //mod fd in epfd.
    void mod(int fd, long long data, __uint32_t event);

    //wait time.
    //return: Number of events triggered.
    int wait(int millseconds);
protected:
    //funtion of Control, change mode LT to ET
    //fd : handle, Assigned when the create function is created
    //data: 
    //event: Event that need to listened.
    //op: EPOLL_CTL_ADD,EPOLL_CTL_MOD,EPOLL_CTL_DEL
    void ctrl(int fd, long long data, __uint32_t event, int op);
protected:
    //epoll
    int _iEpollfd;
    //Maximum number of connections
    int _max_connections;
    //Event set
    struct epoll_event *_pevs;
    //ET or LT
    bool _et;
private:

};
}

