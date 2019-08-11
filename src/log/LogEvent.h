#ifndef __YOUREN_LOGEVENT_H__
#define __YOUREN_LOGEVENT_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <sstream>

#include "LogLevel.h"


namespace youren{

#define YOUREN_LOG_LEVEL(logger, level) \
    if(logger->getLogLevel() <= level){ \
        LogEvent::ptr e(new LogEvent(level, youren::GetThreadId(),youren::GetThreadName(), time(0),\
                                   __FILE__, __LINE__));\
        e->getSS();\
        logger->log(level, e);}

#define YOUREN_LOG_DEBUG(logger) YOUREN_LOG_LEVEL(logger, LogLevel::DEBUG)
#define YOUREN_LOG_INFO(logger) YOUREN_LOG_LEVEL(logger, LogLevel::INFO)
#define YOUREN_LOG_WARN(logger) YOUREN_LOG_LEVEL(logger, LogLevel::WARN)
#define YOUREN_LOG_ERROR(logger) YOUREN_LOG_LEVEL(logger, LogLevel::ERROR)
#define YOUREN_LOG_FATAL(logger) YOUREN_LOG_LEVEL(logger, LogLevel::FATAL)


class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(LogLevel::Level level, uint32_t threadid,const std::string& thread_name,
             uint64_t t, const char* f_name,uint32_t line)
        :m_time(t),m_filename(const_cast<char*>(f_name)), m_line(line) ,m_threadId(threadid),m_threadName(thread_name),m_level(level){}
    ~LogEvent() {}

    uint64_t getTime() const { return m_time; }
    std::string getContent() const { return m_ss.str(); }
    uint32_t getThreadId() const { return m_threadId; }
    LogLevel::Level getLevel() const { return m_level; }
    std::string getLevel(LogLevel::Level level);
    std::stringstream& getSS() { return m_ss; }
    uint32_t getLine() const { return m_line; }
    char* getFileName() const { return m_filename; }
    std::string getThreadName() const { return m_threadName; }
private:
    uint64_t m_time = 0;          //时间戳
    char* m_filename = nullptr;
    uint32_t m_line = 0;
    uint32_t m_threadId = 0;
    std::string m_threadName = nullptr;
    LogLevel::Level m_level;  
    std::stringstream m_ss;
};

}


#endif
