#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

#include <netinet/in.h>
#include "YR_Epoller.h"
using namespace std;
namespace youren{
class NetThread
{
public:
    NetThread();
    virtual ~NetThread();

    int bind(string & ip, int& port);
    void run();
    static void parseAddr(const string &sAddr, struct in_addr & stAddr);
    void createEpoll(uint32_t iIndex = 0);
    bool accept(int fd);
    void processNet(const epoll_event& ev);

    enum{
        ET_LISTEN = 1,
        ET_CLOSE  = 2,
        ET_NOTIFY = 3,
        ET_NET    = 0,
    };

    struct
    {
        string response;
        uint32_t uid;
    }_response;
private:
    int _shutdown_sock;
    int _notify_sock;
    int _sock;
    int ifd;
    YR_Epoller _epoller;
    string _recvbuffer;
    string response;
    map<int, int> _listen_connect_id;
    list<uint32_t> _free;
    volatile size_t _free_size;
};
}
