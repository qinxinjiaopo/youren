#include "LogEvent.h"

using namespace youren;


std::string LogEvent::getLevel(LogLevel::Level level)
{
    switch(level)
    {
    case LogLevel::DEBUG:
        return "DEBUG";break;
    case LogLevel::INFO:
        return "INFO";break;
    case LogLevel::WARN:
        return "WARN";break;
    case LogLevel::ERROR:
        return "ERROR";break;
    case LogLevel::FATAL:
        return "FATAL";break;
    default:
        return "UNKNOW";break;
    }
    return "UNKNOW";
}

