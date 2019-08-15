#pragma once

#include <stdexcept>

using namespace std;
namespace youren {
///////////////////////////////////////////////////////////////////////


class YR_Exception : public exception
{
public:
    YR_Exception(const string& buffer);
    YR_Exception(const string& buffer, int err);

    virtual ~YR_Exception() {}

    //获取错误码
    int getErrorCode();
    //获取异常信息
    virtual const char* what() const throw();

private:
    void getBacktrace() throw();

private:
    //异常的相关信息
    string _buffer;

    //错误码
    int _code;

};
///////////////////////////////////////////////////////////////////////
}

