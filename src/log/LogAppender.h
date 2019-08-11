#pragma once
#include "LogEvent.h"
#include "LogLevel.h"
#include "LogFormatter.h"

namespace youren{
//日志输出方式
class LogAppender
{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    LogAppender() {  }
    virtual ~LogAppender() {}

    virtual void log(LogLevel::Level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr formatter){ m_formatter = formatter; }
    LogFormatter::ptr getFormatter(){ return m_formatter; }
protected:
    LogLevel::Level m_level=LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
};

}
