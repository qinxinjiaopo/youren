#pragma once
#include "LogFormatter.h"

using namespace youren;

class ThreadNameFormatItem : public LogFormatter::FormatItem
{
public:
    ThreadNameFormatItem(const std::string& str="") {}

    void format(std::ostream& os, LogLevel::Level level,LogEvent::ptr event){
        os<<event->getThreadName();
    }
    ~ThreadNameFormatItem() {}

private:

};

