#include "LogEvent.h"
#include "Logger.h"
#include "FileAppender.h"
#include "StdoutAppender.h"

#include "util.h"
using namespace youren;
using namespace std;

int main()
{
    Logger* logger1 = new Logger("youren");

    LogAppender::ptr stdout(new StdoutAppender);
    LogFormatter::ptr fmt(new LogFormatter("%d%T%t%T%N%T<%f:%l>%T[%p]%T%m"));
    stdout->setFormatter(fmt);
    logger1->addAppender(stdout);

    YOUREN_LOG_INFO(logger1)<<"youren test hello";
    /* LogEvent::ptr event(new LogEvent(LogLevel::ERROR, 0,"thread_name",time(0),__FILE__,__LINE__)); */
    /* event->getSS()<<"test youren hello"; */

    /* logger1->log(LogLevel::DEBUG, event); */


    std::string log_name = "log.txt";
   // LogAppender::ptr file_appender(new FileAppender(log_name)); 
    //file_appender->setFormatter(fmt);
    
    /* logger1->addAppender(file_appender); */
    /* logger1->log(LogLevel::DEBUG, event); */
    return 0;
}

