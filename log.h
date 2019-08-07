#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>

namespace youren{

class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent() {}
    ~LogEvent() {}

private:
    uint64_t m_time = 0;          //时间戳
    std::string m_content;        //日志内容
};

class LogLevel{
public:
    enum Level {
        DEBUG = 1,
        INFO  = 2,
        WARN  = 3,
        ERROR = 4,
        FATAL = 5
    };
};

//日志输出格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    ~LogFormatter() {}

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
private:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){  }
        virtual void format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
private:
    std::string m_pattern;
    std::vector<FormatItem> m_items;
};

//日志输出方式
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    LogAppender() {}
    virtual ~LogAppender() {}

    virtual void log(LogLevel::Level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr formatter){ m_formatter = formatter; }
    LogFormatter::ptr getFormatter(){ return m_formatter; }
private:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};


//日志器
class Logger {
public:
    Logger() {}
    ~Logger() {}

    Logger(const std::string& name = "root");
public:
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel::Level getLogLevel() const { return m_level; }
    void setLogLevel(LogLevel::Level val) { m_level = val; }
private:
    std::string m_name;                 //日志名称
    LogLevel::Level m_level;            //日志级别
    std::list<LogAppender> m_appenders; //Appender集合
};


class StdoutAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutAppender> ptr;
    void log(LogLevel::Level, LogEvent::ptr event) override;
private:

};

class FileAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileAppender> ptr;
    FileAppender(const std::string& filename);
    void log(LogLevel::Level, LogEvent::ptr event) override;

    bool reopen();
private:
    std::string m_name;
    std::ostream m_filestream;
};
}


