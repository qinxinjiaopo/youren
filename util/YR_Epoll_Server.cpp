
#include "YR_Epoll_Server.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cassert>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

using namespace std;

namespace youren{
#define H64(x) (((uint64_t)x) << 32)

NetThread::NetThread()
{
    //通知socket和关闭socket
    int iSocketType = SOCK_STREAM;
    int iDomain = AF_INET;

    _shutdown_sock = socket(iDomain, iSocketType, 0);
    _notify_sock = socket(iDomain, iSocketType, 0);

    if(_shutdown_sock < 0)
    {
        cout<<"_shutdown_sock invalid"<<endl;
    }
    if(_notify_sock < 0)
    {
        cout<<"_notify_sock invalid"<<endl;
    }

    _response.response="";
    _response.uid = 0;
}

NetThread::~NetThread()
{
}

int NetThread::bind(string & ip, int& port)
{
    //create listen socket.
    int iSocketType = SOCK_STREAM;
    int iDomain = AF_INET;

    _sock = socket(iDomain, iSocketType, 0);

    if(_sock < 0)
    {
        cout<<"bind_sock invalid"<<endl;
    }
    
    struct sockaddr_in bindAddr;
    bzero(&bindAddr, sizeof(bindAddr));

    bindAddr.sin_family = iDomain;
    bindAddr.sin_port = htons(port);
    parseAddr(ip, bindAddr.sin_addr);

    //如果服务器终止后，服务器第二次可以快速启动，而不用等待一段时间
    int iReuseAddr = 1;
    //允许重用本地地址
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&iReuseAddr, sizeof(int));
    if(::bind(_sock, (struct sockaddr*)(&bindAddr),sizeof(bindAddr)) < 0)
    {
        cout<<"bind error"<<endl;
    }
    cout<<"server already bind fd is "<<_sock<<endl;

    int iConnBackLog = 1024;
    if(::listen(_sock, iConnBackLog) < 0)
    {
        cout<<"listen error"<<endl;
    }
    cout<<"server already listen fd is "<<_sock<<endl;

    int flag = 1;
    //周期性测试连接是否仍存活
    //如果2小时内在该套接字的任一方向每没有数据交换，TCP则自动给对端发送一个保持
    //存活探测分节，这是一个对端必须响应的TCP分节，导致三种情况：
    //  1. 对端进行ACK确认，一切正常。
    //  2. 对端发送RST响应，表示对端已经崩溃且已经重新启动。该套接字的待处理错误
    //  被置为ECONNRESET，套接字本身被关闭。
    if(setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag,int(sizeof(int))) == -1)
    {
        cout<<"setKeepAlive error"<<endl;
    }

    flag = 1;
    //禁止Nagle算法
    if(setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, int(sizeof(int))) == -1)
    {
        cout<<"[YR_Socket::setTcpNoDelay] error"<<endl;
    }

    linger stLinger;
    stLinger.l_onoff = 1;           //在close socket调用后，但是还有数据没有发送完毕的时候容许逗留
    stLinger.l_linger = 0;          //容许逗留的时间为0秒

    if(setsockopt(_sock, SOL_SOCKET, SO_LINGER, (const void*)&stLinger, sizeof(stLinger)) == -1)
    {
        cout<<"[YR_Sockete::setNoCloseWait] error"<<endl;
    }

    //设为非阻塞
    int val = 0;
    bool bBlock = false;
    if((val = fcntl(_sock, F_GETFL, 0)) == -1)
    {
        cout<<"[YR_Socket::setblock] fcntl [F_GETFL] error"<<endl;
    }
    if(!bBlock)
    {
        val |= O_NONBLOCK;
    }
    else
    {
        val &= ~O_NONBLOCK;
    }

    if(fcntl(_sock, F_SETFL, val) == -1)
    {
        cout<<"fcntl nonblock error"<<endl;
    }

    return _sock;
}


}


