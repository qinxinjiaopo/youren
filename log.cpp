/***********************************************
#
#      Filename: log.cpp
#
#        Author: Youren - 986917878@qq.com
#   Description: ---
#        Create: 2019-08-06 11:51:32
# Last Modified: 2019-08-06 11:51:32
***********************************************/
#include "log.h"

Logger::Logger(const string& name)
    :m_name(name){
}

void Logger::log(Loglevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        for(auto i:m_appenders)
            i->log(level, event);
    }
}

void Logger::debug(LogEvent::ptr event)
{
    debug(LogLevel::DEBUG,event);
}

void Logger::info(LogEvent::ptr event)
{
    debug(LogLevel::INFO,event);
}

void Logger::warn(LogEvent::ptr event)
{
    debug(LogLevel::WARN,event);
}

void Logger::error(LogEvent::ptr event)
{
    debug(LogLevel::ERROR,event);
}

void Logger::fatal(LogEvent::ptr event)
{
    debug(LogLevel::FATAL,event);
}

void Logger::addAppender(LogAppender::ptr appender)
{
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender)
{
    for(auto it = m_appenders.begin(); it!=m_appender.end();it++)
    {
        if(*it == appender)
        {
            m_appenders.erase(it);
            break;
        }
    }
}

FileAppender::FileAppender(const std::string& filename)
    :m_name(filename)
{
    
}

bool FileAppender::reopen()
{
    if(m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_name);
    return !!m_filestream;
}
void StdoutAppender::log(LogLevel::Level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        std::cout<< m_formatter->format(event);
    }
}

void FileAppender::log(LogLevel::Level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        m_filestream << m_formatter->format(event);
    }
}

