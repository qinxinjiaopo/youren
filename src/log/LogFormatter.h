#pragma once
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "LogEvent.h"
#include "LogLevel.h"

namespace youren{
class LogFormatter
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(){}
    LogFormatter(const std::string&);
    ~LogFormatter() {}

    std::string format(LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& os,LogLevel::Level level,LogEvent::ptr event);

    void init();
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
private:
    std::string m_pattern = "%d[%p]%m%t";
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};
}
