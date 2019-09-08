#include "YR_Logger.h"
#include <functional>

namespace youren {
LoggerBuffer::LoggerBuffer() : _buffer(NULL), _buffer_len(0)
{
}
//原型：
//void setg( char_type* gbeg, char_type* gcurr, char_type* gend );
//设置定义获取区的指针值。特别是调用后 eback() == gbeg ， gptr() == gcurr ， egptr() == gend 。
//void setp（char * new_pbase，char * new_epptr）;
//设置定义放置区的指针值
LoggerBuffer::LoggerBuffer(YR_LoggerRollPtr roll, size_t buffer_len) : _roll(roll), _buffer(NULL), _buffer_len(buffer_len)
{
	setg(NULL, NULL, NULL);	//无效
    if(_roll)
    {
        _buffer = new char[_buffer_len];
        setp(_buffer, _buffer + _buffer_len);
    }
    else
    {
        setp(NULL,NULL);
        _buffer_len = 0;
    }
}
LoggerBuffer::~LoggerBuffer()
{
    sync();
    if (_buffer)
    {
        delete[] _buffer;
    }
}
streamsize LoggerBuffer::xsputn(const char_type* s, streamsize n)
{
    if (!_roll)
    {
        return n;
    }
    //std::streamsize xsputn( const char_type* s, std::streamsize count );
    //从首元素为 s 所指向的数组写 count 个字符到输出序列
    return std::basic_streambuf<char>::xsputn(s, n);
}

void LoggerBuffer::reserve(std::streamsize n)
{
    //如果小于已开辟的空间大小，则直接返回。
    if( n <= _buffer_len) return;
    if(n > MAX_BUFFER_LENGTH) n =MAX_BUFFER_LENGTH;
    //char_type* pbase() const; 返回指向放置区起始（“基”）的指针。
	//char_type* pptr() const;	返回指向放置区中当前字符的指针（放置指针）。
	//char_type* epptr() const; 返回指向放置区结尾后一位置的指针。
    //重新设置_buffer
    int len = pptr() - pbase();
    char_type * p = new char_type[n];
    //将原来数据复制到新的空间中，释放原来的空间
    memcpy(p, _buffer, len);
    delete[] _buffer;
    _buffer = p;
    _buffer_len = n;
    //设置buffer的起始指针和末尾指针
    setp(_buffer, _buffer + _buffer_len);
    //重寻位放置指针（ pptr() ） len 个字符
    pbump(len);
    return;
}
std::basic_streambuf<char>::int_type LoggerBuffer::overflow(std::basic_streambuf<char>::int_type c)
{
    if (!_roll)return 0;
    if (_buffer_len >= MAX_BUFFER_LENGTH)
    {
        sync();
    }
    else
    {
        reserve(_buffer_len * 2);
    }
    if (std::char_traits<char_type>::eq_int_type(c,std::char_traits<char_type>::eof()) )
    {
        return std::char_traits<char_type>::not_eof(c);
    }
    else
    {
        //写入一个字符到字符序列，如果buffer已满，调用overflow()
        return sputc(c);
    }
    return 0;
}
//重写了std::basic_streambuf< char >中的sync方法
//当调用flush()时，就会调用sync()
int LoggerBuffer::sync()
{
        //当前指针的值大于其实指针的值，说明_buffer中有数据
    if (pptr() > pbase())
    {
        std::streamsize len = pptr() - pbase();
        if (_roll)
        {
            //具体的写逻辑
            _roll->write(make_pair(0, string(pbase(), len)));
        }
        //重新设置put缓冲区, pptr()重置到pbase()处
        setp(pbase(), epptr());
    }
    return 0;
}

YR_LoggerThreadGroup::~YR_LoggerThreadGroup()
{
    flush();
    _bTerminate = true;
    
    Lock lock(*this);
    notifyAll();
    _tpool.stop();
    _tpool.waitForAllDone();
}

void YR_LoggerThreadGroup::start(size_t tNum)
{
    _tpool.init(tNum);
    _tpool.start();
    
    auto func = std::bind(&YR_LoggerThreadGroup::run, this);
    for(size_t i =0; i< _tpool.getThreadNum(); i++)
    {
        _tpool.exec(func);
    }
}

void YR_LoggerThreadGroup::registerLogger(YR_LoggerRollPtr &l)
{
    Lock lock(*this);
    _logger.insert(l);
}

void YR_LoggerThreadGroup::unregisterLogger(YR_LoggerRollPtr& l)
{
    Lock lock(*this);
    _logger.erase(l);
}

void YR_LoggerThreadGroup::flush()
{
cout<<"LoggerThreadGroup::flush() "<<__FILE__<<__LINE__<<endl;
    set<YR_LoggerRollPtr,Comp> loggers;
    Lock lock(*this);
    loggers = _logger;

    set<YR_LoggerRollPtr,Comp>::iterator it =loggers.begin();
    while(it != loggers.end())
    {
        try
        {
            it->get()->flush();
        }
        catch(...)
        {}
        ++it;
    }
}

void YR_LoggerThreadGroup::run()
{
    cout<<"LoggerThreadGroup::run()"<<endl;
    cout<<"线程ID"<<pthread_self()<<endl;
    while(!_bTerminate)
    {
            Lock lock(*this);
            timeWait(100);
        flush();
    }
}


void YR_LoggerRoll::setupThread(YR_LoggerThreadGroup *pThreadGroup)
{
cout<<"YR_LoggerRoll::setupThread()"<<endl;
    assert(pThreadGroup != NULL);
    unSetupThread();
cout<<"YR_LoggerRoll::setupThread()"<<endl;
    YR_LockT<YR_ThreadMutex> lock(_mutex);
    _pThreadGroup = pThreadGroup;
    YR_LoggerRollPtr self = this;
    _pThreadGroup->registerLogger(self);
}

void YR_LoggerRoll::unSetupThread()
{
cout<<"LoggerRoll::unSetupThread()"<<endl;
    YR_LockT<YR_ThreadMutex> lock(_mutex);
    if(_pThreadGroup != NULL)
    {
        _pThreadGroup->flush();
        YR_LoggerRollPtr self =this;
        _pThreadGroup->unregisterLogger(self);
        _pThreadGroup = NULL;
    }
    flush();
cout<<"LoggerRoll::unSetupThread()"<<endl;
}

void YR_LoggerRoll::write(const pair<int, string> &buffer)
{
    pthread_t threadId = 0;
    if(_pThreadGroup)
    {
        _buffer.push_back(make_pair(threadId,buffer.second));
    }
    else
    {
        deque<pair<int, string>> ds;
        ds.push_back(make_pair(threadId, buffer.second));
        roll(ds);
    }
}

void YR_LoggerRoll::flush()
{
cout<<"LoggerRoll::flush()"<<endl;
    YR_Thread_Queue<pair<int, string>>::queue_type qt;
    _buffer.swap(qt);
    if(!qt.empty())
    {
cout<<"进入roll"<<endl;
        roll(qt);
    }
}


}
