#pragma once
#include "LogFormatter.h"

using namespace youren;
class LineFormatItem : public  LogFormatter::FormatItem
{
public:
    LineFormatItem(const std::string& str="") {}
    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os<<event->getLine();
    }
    ~LineFormatItem() {}

private:

};

