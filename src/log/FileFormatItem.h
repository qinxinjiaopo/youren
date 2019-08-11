#pragma once
#include "LogFormatter.h"

using namespace youren;

class FileFormatItem : public LogFormatter::FormatItem
{
public:
    FileFormatItem(const std::string& str = "") {}
    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os<<event->getFileName();
    }
private:

};

