#include "StdoutAppender.h"

namespace youren{

void StdoutAppender::log(LogLevel::Level level, LogEvent::ptr event) 
{
    if(level >= m_level)
    {
        std::cout<< m_formatter->format(level, event);
    }
}

}
