#pragma once
#include "util/YR_Logger.h"
#include "util/YR_File.h"
#include "util/YR_Singleton.h"

#define DYEING_DIR "youren_dyeing"
#define DYEING_FILE "dyeing"

namespace youren
{
class RollWriteT
{
public:
    RollWriteT();
    ~RollWriteT();

    void operator()(ostream &of, const deque<pair<int, string> > &ds);

protected:
    string _app;
    string _server;
    string _logPath;
    int _maxSize;
    int _maxNum;
};

class YRRollLogger : public YR_Singleton<YRRollLogger, CreateUsingNew, PhoneixLifetime>
{
public:
    public:
    typedef YR_Logger<RollWriteT ,YR_RollBySize> RollLogger;
   void setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize = 1024*1024*50, int iMaxNum = 10);

    void sync(bool bSync = true);

    RollLogger *logger()          { return &_logger; }
    void enableDyeing(bool bEnable, const string& sDyeingKey = "");

protected:
    string                  _app;
    string                  _server;
    string                  _logpath;
    RollLogger              _logger;
    YR_LoggerThreadGroup    _local;
};

///////////////////////////////////////////////////////////////////////////////////////
//写日志线程
class YRLoggerThread : public YR_Singleton<YRLoggerThread, CreateUsingNew, PhoneixLifetime>
{
public:
    YRLoggerThread();
    ~YRLoggerThread();
    YR_LoggerThreadGroup* local();
protected:
    YR_LoggerThreadGroup    _local;
};

}



