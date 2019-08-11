#pragma once
#include <iostream>
#include "LogAppender.h"

namespace youren{

class StdoutAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutAppender> ptr;
    void log(LogLevel::Level, LogEvent::ptr event) override;
private:

};

}
