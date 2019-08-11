#pragma once
#include "LogFormatter.h"

using namespace youren;

class StringFormatItem : public LogFormatter::FormatItem
{
public:
    StringFormatItem(const std::string& str)
        :m_string(str){}

    
    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os<<m_string;
    }
private:
    std::string m_string;
};

