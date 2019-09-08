#pragma once
#include <string>
#include <string.h>
#include "YR_Atomic.h"
#include "YR_Exception.h"

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

    void incRef() {_atomic.inc_fast();}
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
            _ptr->incRef();
        }
    }

    //用其他智能指针的原始指针初始化，计数+1
    template<class Y>
    YR_AutoPtr(const YR_AutoPtr<Y>& t)
    {
        _ptr = t._ptr;
        if(_ptr)
        {
            _ptr->incRef();
        }
    }

    YR_AutoPtr(const YR_AutoPtr& t)
    {
        _ptr = t._ptr;
        if(_ptr)
        {
            _ptr->incRef();
        }
    }

    ~YR_AutoPtr() {
        if(_ptr)
        {
            _ptr->decRef();
        }
    }

    //赋值，普通指针
    YR_AutoPtr& operator=(T* p)
    {
        if(_ptr != p)
        {
            if(p)
            {
                p->incRef();
            }

            T* ptr=_ptr;
            _ptr = p;

            //释放原来指针
            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    //赋值，其他类型智能指针
    template<class Y>
    YR_AutoPtr& operator=(const YR_AutoPtr<Y>& r)
    {
        if(_ptr != r._ptr){
            if(r._ptr)
            {
                r._ptr->incRef();
            }

            T* ptr = _ptr;
            _ptr = r._ptr;
            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    YR_AutoPtr& operator=(const YR_AutoPtr& r)
    {
        if(_ptr != r._ptr)
        {
            if(r._ptr)
            {
                r._ptr->incRef();
            }
            
            T* ptr = _ptr;
            _ptr = r._ptr;
            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    //将其他类型的指针转换为当前类型的指针
    template<class Y>
    static YR_AutoPtr dynamicCast(const YR_AutoPtr<Y> & r)
    {
        return YR_AutoPtr(dynamic_cast<T*>(r._ptr));
    }

    //将其他类型的原生指针转换为当前类型的智能指针
    template<class Y>
    static YR_AutoPtr dynamicCast(Y* p)
    {
        return YR_AutoPtr(dynamic_cast<T*>(p));
    }

    //获取原生指针
    T* get() const
    {
        return _ptr;
    }

    //调用
    T* operator->() const
    {
        if(!_ptr)
        {
            throwNullHandleException();
        }
        return _ptr;
    }

    T& operator*() const
    {
        if(!_ptr)
        {
            throwNullHandleException();
        }
        return *_ptr;
    }

    //是否有效
    operator bool() const
    {
        return _ptr?true:false;
    }

    //交换指针
    void swap(YR_AutoPtr& other)
    {
        std::swap(_ptr, other._ptr);
    }

protected:
    //抛出异常
    void throwNullHandleException() const;

public:
    T* _ptr;
private:

};

template<class T> inline void 
YR_AutoPtr<T>::throwNullHandleException() const
{
    throw YR_AutoPtrNull_Exception("autoptr null handle error");
}

// == 判断
template<class T, class U> inline bool
operator==(const YR_AutoPtr<T>& lhs, const YR_AutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();

    if(l && r)
    {
        return *l == *r;
    }
    else
    {
        return !l && !r;
    }
}

//不等于判断
template<class T, class U> inline bool
operator!=(const YR_AutoPtr<T>& lhs, const YR_AutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();

    if(l && r)
    {
        return *l != *r;
    }
    else
    {
        return l || r;
    }
}

//小于判断，用于map容器中
template<class T, class U>inline bool
operator<(const YR_AutoPtr<T>& lhs,const YR_AutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();

    if(l && r)
    {
        return *l < *r;
    }
    else
    {
        return !l && r;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
}
