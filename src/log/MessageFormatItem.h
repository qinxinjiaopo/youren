#pragma once
#include "LogFormatter.h"
#include "LogLevel.h"

using namespace youren;

class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    MessageFormatItem(const std::string& str = ""){  }
    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os << event->getContent();
    }
private:

};

