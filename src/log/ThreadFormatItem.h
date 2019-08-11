#pragma once
#include "LogFormatter.h"

using namespace youren;

class ThreadFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadFormatItem(const std::string& str="") {}
    
    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os << event->getThreadId();
    }
private:

};

