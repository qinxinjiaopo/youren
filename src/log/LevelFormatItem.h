#pragma once
#include "LogFormatter.h"
#include"LogLevel.h"

using namespace youren;

class LevelFormatItem : public LogFormatter::FormatItem
{
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os << event->getLevel(level);
    }
private:
};

