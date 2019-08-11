#pragma once
#include "LogFormatter.h"
#include <ctime>

#include <fstream>
using namespace youren;

class DateTimeFormatItem : public LogFormatter::FormatItem
{
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format){
            if(m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

    void format(std::ostream& os,LogLevel::Level level, LogEvent::ptr event) {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }

private:
    std::string m_format;
};

