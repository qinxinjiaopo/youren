#include "YRLogger.h"
#include <iostream>

using namespace std;

namespace youren
{
RollWriteT::RollWriteT():_maxSize(10000), _maxNum(1)
{
}

RollWriteT::~RollWriteT()
{
}

void RollWriteT::operator()(ostream &of, const deque<pair<int, string> > &ds)
{
    vector<string> vRemoteDyeing;

    deque<pair<int, string> >::const_iterator it = ds.begin();
    while(it != ds.end())
    {
        of << it->second;

        ++it;
    }
    of.flush();
}

/////////////////////////////////////////////////////////////////////////////////////

void YRRollLogger::setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize, int iMaxNum)
{
    _app       = sApp;
    _server    = sServer;
    _logpath   = sLogpath;

    //生成目录
    YR_File::makeDirRecursive(_logpath + "/" + _app + "/" + _server);

	cout<<"start threadpool"<<endl;
    _local.start(1);

    //初始化本地循环日志
	cout<<"_logger.init"<<endl;
    _logger.init(_logpath + "/" + _app + "/" + _server + "/" + _app + "." + _server, iMaxSize, iMaxNum);

    //设置为异步
    sync(false);
}


void YRRollLogger::sync(bool bSync)
{
    if(bSync)
    {
        _logger.unSetupThread();
    }
    else
    {
        _logger.setupThread(&_local);
    }
}
/////////////////////////////////////////////////////////////////////////////////////

YRLoggerThread::YRLoggerThread()
{
    _local.start(1);
}

YRLoggerThread::~YRLoggerThread()
{
    _local.flush();
}

YR_LoggerThreadGroup* YRLoggerThread::local()
{
    return &_local;
}

}
