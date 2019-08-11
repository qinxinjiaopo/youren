#ifndef __YOUREN_UTIL_H__
#define __YOUREN_UTIL_H__

#include <cxxabi.h>
#include <thread>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

namespace youren
{
//获取线程id
pid_t GetThreadId();

std::string GetThreadName();

}
#endif
