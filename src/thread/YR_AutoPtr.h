#pragma once
#include <string>
#include <string.h>
#include "YR_Atomic.h"
#include "../ex/YR_Exception.h"

namespace youren{
////////////////////////////////////////////////////////////////////////////////////////

//指针异常类
struct YR_AutoPtrNull_Exception : public YR_Exception
{
    YR_AutoPtrNull_Exception(const string& buffer) : YR_Exception(buffer){}
    ~YR_AutoPtrNull_Exception() throw(){}
};

//智能指针基类
template <class T>
class YR_HandleBaseT
{
public:
    typedef T atomic_type;
    YR_HandleBaseT& operator=(const YR_HandleBaseT&) { return *this; }

    void incRef() {_atomic.inc_fase();}
    void decRef()
    {
        if(_atomic.dec_and_test() && !_bNoDelete)
        {
            _bNoDelete = true;
            delete this;
        }
    }

    int getRef() const { return _atomic.get(); }
    void setNoDelete(bool b) { _bNoDelete = b; }

protected:
    YR_HandleBaseT():_atomic(0), _bNoDelete(false){}
    YR_HandleBaseT(const YR_HandleBaseT&) :_atomic(0),_bNoDelete(false){}

    virtual ~YR_HandleBaseT(){}

    atomic_type _atomic;
    bool _bNoDelete;
};

template<>
inline void YR_HandleBaseT<int>::incRef()
{
    ++_atomic;
}

template<>
inline void YR_HandleBaseT<int>::decRef()
{
    if(--_atomic == 0 && !_bNoDelete)
    {
        _bNoDelete = true;
        delete this;
    }
}

template<>
inline int YR_HandleBaseT<int>::getRef() const
{
    return _atomic;
}

//智能指针模板类
//可以放在容器中，且线程安全的智能指针

typedef YR_HandleBaseT<YR_Atomic> YR_HandleBase;

template <class T> //T必须继承YR_HandleBase
class YR_AutoPtr
{
public:
    typedef T element_type;

    //用原生指针初始化，计数+1
    YR_AutoPtr(T* p = 0) {
        _ptr = p;
        if(_ptr)
        {
            _ptr.incRef();
        }
    }
    ~YR_AutoPtr() {}

private:

};
/////////////////////////////////////////////////////////////////////////////////////////
}
