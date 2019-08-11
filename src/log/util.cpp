#include "util.h"

namespace youren
{
//获取线程id
pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}

std::string GetThreadName()
{
    char *name=(char*)malloc(sizeof(char)*64);
    prctl(PR_GET_NAME,(unsigned long)name);
    return name;
}

}
