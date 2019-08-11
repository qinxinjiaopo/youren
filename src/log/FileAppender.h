#include <sstream>
#include <fstream>

#include "LogAppender.h"

namespace youren{

class FileAppender : public LogAppender 
{
public:
    typedef std::shared_ptr<FileAppender> ptr;
    FileAppender(const std::string& filename);
    void log(LogLevel::Level, LogEvent::ptr event) override;

    bool reopen();
private:
    std::string m_name;
    std::ofstream m_filestream;
};
}
