#include "FileAppender.h"
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

namespace youren {

FileAppender::FileAppender(const std::string& filename)
    :m_name(filename)
{
    reopen();
}

void FileAppender::log(LogLevel::Level level, LogEvent::ptr event)
{
    if(level >= m_level)
    {
        if(!m_formatter->format(m_filestream, level, event))
        {
            std::cout<<"error"<<std::endl;
        }
    }
}

bool FileAppender::reopen()
{
    if(m_filestream)
    {
        m_filestream.close();
    }
    m_filestream.open(m_name.c_str(), std::ios::app);
    if(!m_filestream.is_open())
    {
        std::string dir;
        //解析文件目录名称
        auto pos = m_name.rfind('/');
        if(pos == 0)
        {
            dir = "/";
        }
        else if(pos == std::string::npos)
        {
            dir = ".";
        }
        else
        {
            dir = m_name.substr(0, pos);
        }
        //创建目录
        char* path = strdup(dir.c_str());
        char* ptr = strchr(path + 1, '/');
        do {
            for(; ptr; *ptr = '/', ptr = strchr(ptr + 1, '/')) {
                *ptr = '\0';
                if(!access(path, F_OK))
                {
                    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                    break;
                }
            }
            if(ptr != nullptr) {
                break;
            } else if(access(path, F_OK)!= 0) {
                mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                break;
            }
            free(path);break;
        } while(0);

        m_filestream.open(m_name.c_str(),std::ios::app);
    }
    return m_filestream.is_open();
}


}
