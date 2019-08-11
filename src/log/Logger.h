#pragma once
#include <list>

#include "LogAppender.h"

namespace youren{

class Logger
{
public:
    Logger(const std::string& name);
    ~Logger() {}
public:
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel::Level getLogLevel() const { return m_level; }
    void setLogLevel(LogLevel::Level val) { m_level = val; }
private:
    std::string m_name;                 //日志名称
    LogLevel::Level m_level;            //日志级别
    std::list<LogAppender::ptr> m_appenders; //Appender集合
};

}
