#pragma once
#include <stdint.h>

namespace youren {
///////////////////////////////////////////////////////////////////////////////////

//原子计数类
__BEGIN_DECLS
#define YR_LOCK "lock ;"
    typedef struct { volatile int counter; } youren_atomic_t;
#define youren_atomic_read(v) ((v)->counter)

#define youren_atomic_set(v,i) (((v)->counter) = (i))

__END_DECLS

//原子操作类
class YR_Atomic
{
public:
    //原子类型
    typedef int atomic_type;

    YR_Atomic(atomic_type at =0) { set(at); }
    YR_Atomic& operator++()
    {
        inc();
        return *this;
    }
    
    YR_Atomic& operator--()
    {
        dec();
        return *this;
    }

    operator atomic_type() const 
    {
        return get();
    }

    YR_Atomic& operator+=(atomic_type n)
    {
        add(n);
        return *this;
    }
    YR_Atomic& operator-=(atomic_type n)
    {
        sub(n);
        return *this;
    }
    YR_Atomic& operator=(atomic_type n)
    {
        set(n);
        return *this;
    }

    //获取值
    atomic_type get() const { return _value.counter; }
    atomic_type add(atomic_type i) { return add_and_return(i); }
    atomic_type sub(atomic_type i) { return add_and_return(-i); }
    atomic_type inc() { return add(1); }
    atomic_type dec() { return sub(1); }
    void inc_fast()
    {
        __asm__ __volatile__(
                            YR_LOCK "incl %0"
                            :"=m" (_value.counter)
                            :"m" (_value.counter)
                            );
    }

    bool dec_and_test()
    {
        unsigned char c;
        __asm__ __volatile__(
                             YR_LOCK "decl %0;sete %1"
                             :"=m" (_value.counter), "=qm" (c)
                             :"m" (_value.counter) :"memory"
                            );
        return c != 0;
    }

    atomic_type set(atomic_type i)
    {
        _value.counter = i; 
        return i;
    }
    ~YR_Atomic() {}
protected:
    int add_and_return(int i)
    {
        int __i =i;
        __asm__ __volatile__(
                            YR_LOCK "xaddl %0, %1"
                            :"=r" (i)
                            :"m"(_value.counter), "0"(i)
                            );
        return i+ __i;
    }

    youren_atomic_t _value;
private:

};
///////////////////////////////////////////////////////////////////////////////////
}
