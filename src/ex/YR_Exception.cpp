#include "YR_Exeption.h"
#include <cerrno>
#include <execinfo.h>
#include <cstring>
#include <cstdlib>

namespace youren{

YR_Exception::YR_Exception(const string& buffer)
    :_buffer(buffer), _code(0)
{

}

YR_Exception::YR_Exception(const string& buffer, int err)
{
    _buffer = buffer + " :" + strerror(err);
    _code = err;
}

int YR_Exception::getErrorCode()
{
    return _code;
}

void YR_Exception::getBacktrace() throw()
{
    void * array[64];
    int nSize = backtrace(array, 64);
    char** symbols = backtrace_symbols(array, nSize);

    for(int i =0; i<nSize; i++)
    {
        _buffer += symbols[i];
        _buffer += "\n";
    }
    free(symbols);
}

const char* YR_Exception::what() const throw()
{
    return _buffer.c_str();
}

}
