#pragma once
#include "LogFormatter.h"

using namespace youren;

class TabFormatItem : public LogFormatter::FormatItem
{
public:
    TabFormatItem(const std::string& str="")
        :m_string(str){}

    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        os<<"\t";
    }
private:
    std::string m_string;
};

