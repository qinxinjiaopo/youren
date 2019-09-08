#pragma once
#include "YR_AutoPtr.h"
#include "YR_Lock.h"
#include "YR_Monitor.h"
#include "YR_Thread_Queue.h"
#include "YR_ThreadPool.h"
#include "YR_TimeProvider.h"
#include "YR_File.h"

#include <iostream>
#include <ext/hash_map>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>


namespace youren {

class YR_LoggerThreadGroup;
//日志异常类
struct YR_Logger_Exception : public YR_Exception
{
    YR_Logger_Exception(const string &buffer) : YR_Exception(buffer){};
    YR_Logger_Exception(const string &buffer, int err) : YR_Exception(buffer, err){};
    ~YR_Logger_Exception() throw(){};
};
/////////////////////////////////////////////////////////////////////////////////////////////////
//日志滚动方法基类
class YR_LoggerRoll : public YR_HandleBase
{
public:
    YR_LoggerRoll() : _pThreadGroup(NULL)
    {
    }

    virtual void roll(const deque<pair<int, string> > &ds) = 0;
    void setupThread(YR_LoggerThreadGroup *pThreadGroup);
    void unSetupThread();
    void write(const pair<int, string> &buffer);
    void flush();
protected:
    YR_Thread_Queue<pair<int, string> >  _buffer;
    YR_ThreadMutex          _mutex;
    YR_LoggerThreadGroup    *_pThreadGroup;
    static bool             _bDyeingFlag;
    static YR_ThreadMutex    _mutexDyeing;
};

typedef YR_AutoPtr<YR_LoggerRoll> YR_LoggerRollPtr;
///////////////////////////////////////////////////////////////////////////////////////////////////

//日志buffer的封装
//继承basic_streambuf
class LoggerBuffer : public std::basic_streambuf<char>
{
public:
    const int MAX_BUFFER_LENGTH = 1024*1024*10;		//定义最大buffer的空间为10M
    LoggerBuffer();
    LoggerBuffer(YR_LoggerRollPtr roll, size_t buffer_size);
    ~LoggerBuffer();

protected:
    //分配空间
    void reserve(std::streamsize n);
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n);
    //处理buffer不足的情况
    virtual int_type overflow(int_type c);
    //处理读buffer为空的情况
    virtual int underflow()	{ return std::char_traits<char_type>::eof();}
    //写逻辑i,重写
    virtual int sync();
protected:
    LoggerBuffer(const LoggerBuffer&) = delete;
    LoggerBuffer& operator=(const LoggerBuffer&) = delete;
protected:
    YR_LoggerRollPtr _roll;						//写日志
    char * _buffer;								//缓冲区,这里使用字符数组实现
    std::streamsize _buffer_len;				//缓冲区大小
};
//////////////////////////////////////////////////////////////////////////////////////////////////

//日志线程组的封装实现
class YR_LoggerThreadGroup : public YR_ThreadLock
{
public:
    YR_LoggerThreadGroup():_bTerminate(false){  }
    ~YR_LoggerThreadGroup();
    void start(size_t tNum);                    //启动线程
    void registerLogger(YR_LoggerRollPtr &l);   //添加logger对象
    void unregisterLogger(YR_LoggerRollPtr& l); //删除logger对象
    void flush();
protected:
    void run();

protected:
    //为了保持线程池中logger不重复，则使用set集合保存logger
    //Comp为set的比较函数
    struct Comp{
        bool operator()(const YR_LoggerRollPtr& p1,const YR_LoggerRollPtr& p2)
        {
            return p1.get() <p2.get();
        }
    };
private:
    YR_ThreadPool _tpool;
    bool _bTerminate;
    set<YR_LoggerRollPtr, Comp> _logger;
};
//////////////////////////////////////////////////////////////////////////////////////////////
class LoggerStream
{
public:
    /**
     * @brief 构造.
     *
     * @param stream
     * @param mutex
     */
    LoggerStream(const char *header, ostream* stream, ostream *estream, YR_ThreadRecMutex &mutex) : _stream(stream), _estream(estream), _mutex(mutex)
    {
        _buffer << header;
    }

    /**
     * @brief 析构
     */
    ~LoggerStream()
    {
        if (_stream)
        {
            YR_LockT<YR_ThreadRecMutex> lock(_mutex);
            _stream->clear();
            (*_stream) << _buffer.str();

            _stream->flush();
        }
    }

    /**
     * @brief 重载<<
     */
    template <typename P>
        LoggerStream& operator << (const P &t)  { if (_stream) _buffer << t;return *this;}

    /**
     * @brief endl,flush等函数
     */
    typedef ostream& (*F)(ostream& os);
    LoggerStream& operator << (F f)         { if (_stream) (f)(_buffer);return *this;}

    /**
     * @brief  hex等系列函数
     */
    typedef ios_base& (*I)(ios_base& os);
    LoggerStream& operator << (I f)         { if (_stream) (f)(_buffer);return *this;}

    /**
     * @brief 字段转换成ostream类型.
     *
     * @return ostream&
     */
    operator ostream&()
    {
        if (_stream)
        {
            return _buffer;
        }

        return *_estream;
    }

    //不实现
    LoggerStream(const LoggerStream& lt);
    LoggerStream& operator=(const LoggerStream& lt);

protected:
    std::stringstream _buffer;
    std::ostream    *_stream;
    std::ostream    *_estream;
    YR_ThreadRecMutex  &_mutex;
};
//日志类
template <class WriteT, template<class> class RollPolicy>
class YR_Logger : public RollPolicy<WriteT>::RollWrapperI
{
public:
    //日志等级
    enum
    {
        NONE  = 1,
        ERROR = 2,
        WARN  = 3,
        DEBUG = 4,
        INFO  = 5
    };
    static const string LN[6];

public:
    YR_Logger()
        :m_level(DEBUG),m_buffer(YR_LoggerRollPtr::dynamicCast(this->_roll), 1024),_stream(&m_buffer)
         , m_ebuffer(NULL, 0), _estream(&m_ebuffer)
    { }
    ~YR_Logger(){ }

    int getLogLevel() const { return m_level;}
    int setLogLevel(int level)
    {
        switch(level)
        {
        case NONE:
        case ERROR:
        case WARN:        
        case DEBUG:
        case INFO:
            break;
        default:
            return -1;
        }
        m_level = level;
        return 0;
    }
    int setLogLevel(const string& level)
    {
        if(level == "NONE")return setLogLevel(NONE);
        else if(level == "ERROR")return setLogLevel(ERROR);
        else if(level == "WARN")return setLogLevel(WARN);
        else if(level == "DEBUG")return setLogLevel(DEBUG);
        else if(level == "INFO")return setLogLevel(INFO);
        return -1;
    }
    bool IsNeedLog(const string& level)
    {
        if(level == "NONE")return NONE <= m_level;
        else if(level == "ERROR")return ERROR <= m_level;
        else if(level == "WARN")return WARN <= m_level;
        else if(level == "DEBUG")return DEBUG <= m_level;
        else if(level == "INFO")return INFO <= m_level;
        else return true;
    }
    LoggerStream stream(int level) 
    {
        ostream *ost=NULL;
        if (level <= m_level)
        {
            char c[128]="\0";
            int len=127;
            size_t n = 0;
            struct timeval t;
            gettimeofday(&t, NULL);
            tm tt;
            localtime_r(&t.tv_sec, &tt);
            const char* format="[%04d-%02d-%02d %02d:%02d:%02d]%s";
            n += snprintf(c+n, len-n, format, tt.tm_year + 1900, tt.tm_mon +1, tt.tm_mday,tt.tm_hour,tt.tm_min, tt.tm_sec,"|");
            //输出线程ID
            n += snprintf(c + n, len - n, "%ld%s", syscall(SYS_gettid), "|");
            //输出日志等级
            n += snprintf(c+n , len-n, "%s%s", YR_Logger::LN[level].c_str(), "|");

            ost = &_stream;

            return LoggerStream(c, ost, &_estream, m_mutex);
        }

        return LoggerStream(NULL, ost, &_estream, m_mutex);
    }

    LoggerStream error() { return stream(ERROR);}
    LoggerStream warn() { return stream(WARN);}
    LoggerStream info() { return stream(INFO);}
    LoggerStream debug() {
        return stream(DEBUG);}
    LoggerStream any() { return stream(0);}
    LoggerStream log(int level) { return stream(level);}
protected:
    int m_level;
    LoggerBuffer m_buffer;
    std::ostream _stream;
    LoggerBuffer m_ebuffer;
    std::ostream   _estream;
    YR_ThreadRecMutex m_mutex;
};
template <class WriteT, template<class>class RollPolicy>
const string YR_Logger<WriteT, RollPolicy>::LN[6] = {"ANY", "NONE_LOG", "ERROR", "WARN", "DEBUG", "INFO"};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename RollPolicyWriteT>	//YR_RollBySize<WriteT>
class RollWrapperBase
{
public:
    typedef YR_AutoPtr<RollPolicyWriteT>     RollPolicyWriteTPtr;
    RollWrapperBase()
    {
        _roll = new RollPolicyWriteT;
    }
    virtual LoggerStream info() = 0;
    virtual LoggerStream debug() = 0;
    virtual LoggerStream warn() = 0;
    virtual LoggerStream error() = 0;
    virtual LoggerStream any() = 0;
    virtual LoggerStream log(int level)=0;
    void setupThread(YR_LoggerThreadGroup *ltg) { _roll->setupThread(ltg);}
    void unSetupThread()                        { _roll->unSetupThread();}
    typename RollPolicyWriteT::T &getWriteT()   { return _roll->getWriteT();}
    void roll(const pair<int, string> &buffer)  { _roll->write(buffer);}
    RollPolicyWriteTPtr & getRoll()              {return _roll;}
    void flush()                                { _roll->flush(); }
protected:
    RollPolicyWriteTPtr      _roll;

};
//按文件大小日志滚动方法
template <class WriteT>
class YR_RollBySize : public YR_LoggerRoll, public YR_ThreadMutex
{
public:
    typedef WriteT T;
    class RollWrapperI : public RollWrapperBase<YR_RollBySize<WriteT> >
    {
    public:
        void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
        {
            this->_roll->init(path, iMaxSize, iMaxNum);
        }
        string getPath()                    { return this->_roll->getPath();}
        void setPath(const string &path)    { this->_roll->setPath(path);}
        int getMaxSize()                    { return this->_roll->getMaxSize();}
        void setMaxSize(int maxSize)        { this->_roll->setMaxSize(maxSize);}
        int getMaxNum()                     { return this->_roll->getMaxNum();}
        void setMaxNum(int maxNum)          { this->_roll->setMaxNum(maxNum);}
    };
    YR_RollBySize(): m_maxSize(5000000), m_maxNum(10),m_lt(time(NULL))
    {}
    ~YR_RollBySize(){
        if(m_ofs.is_open()) m_ofs.close();
    }
    void init(const string& path, int32_t iMaxSize = 5000000, int iMaxNum = 10)
    {
        cout<<"YR_RollBySize init"<<endl;
        YR_LockT<YR_ThreadMutex> lock(*this);
        m_path = path;
        m_maxSize =iMaxSize;
        m_maxNum = iMaxNum;
    }
    string getPath() { YR_LockT<YR_ThreadMutex> lock(*this); return m_path; }
    void setPath(const string& path){  YR_LockT<YR_ThreadMutex> lock(*this);m_path = path;  }
    int32_t getMaxSize(){  YR_LockT<YR_ThreadMutex> lock(*this);return m_maxSize;  }
    void setMaxSize(int32_t size){  YR_LockT<YR_ThreadMutex> lock(*this);m_maxSize = size;  }
    int32_t getMaxNum(){  YR_LockT<YR_ThreadMutex> lock(*this);return m_maxNum; }
    void setMaxNum(int32_t num){ YR_LockT<YR_ThreadMutex> lock(*this); m_maxNum = num; }

    WriteT& getWriteT(){ return m_t; }
    void roll(const deque<pair<int, string>> &buffer)
    {
        cout<<"RollBySize::roll()"<<endl;
        YR_LockT<YR_ThreadMutex> lock(*this);
        if(m_path.empty())
        {
            m_t(cout, buffer);
            return;
        }
        time_t t= TNOW;
        time_t tt = t - m_lt;
        //每隔5秒重新打开文件
        if( tt >5 || tt<0 )
        {
            m_lt = t;
            m_ofs.close();
        }

        if(!m_ofs.is_open())
        {
            string fileName = m_path + ".log";
            m_ofs.open(fileName.c_str(), ios::app);
            string filePath = YR_File::extractFilePath(m_path);
            if(YR_File::isFileExist(filePath, S_IFDIR))
            {
                YR_File::makeDirRecursive(filePath);
            }
            if(!m_ofs)
            {
                m_t(m_ofs, buffer);
                throw YR_Logger_Exception("[YR_RollBySize::roll] fopen fail:"+fileName, errno);
            }
        }
        m_t(m_ofs, buffer);
        if(tt <= 5)
        {
            return;
        }
        if(m_ofs.tellp() < m_maxSize)return;
        //文件大小超出限制，删除最后一个文件
        string fileName = m_path + to_string(m_maxNum -1)+".log";
        if(access(fileName.c_str(), F_OK) == 0)
        {
            if(remove(fileName.c_str()) < 0)return;
        }

        //将文件命名
        for(int i =m_maxNum -2;i >=0; i--)
        {
            if( i == 0 )
                fileName = m_path + ".log";
            else
                fileName = m_path + to_string(i) + ".log";
            cout<<fileName<<endl;
            if(access(fileName.c_str(), F_OK) == 0)
            {
                string nFileName = m_path + to_string(i+1) +".log";
                rename(fileName.c_str(), nFileName.c_str());
            }
        }
        m_ofs.close();
        m_ofs.open(fileName.c_str(), ios::app);
        if(!m_ofs)
        {
            throw YR_Logger_Exception("[YR_RollBySize::roll] fopen fail:"+fileName, errno);
        }
    }

private:
    string m_path;
    int32_t m_maxSize;        //文件大小
    int32_t m_maxNum;         //文件最大数
    ofstream m_ofs;           //文件
    WriteT m_t;               //具体写操作
    time_t m_lt;              //多长时间检查文件一次
};
}
