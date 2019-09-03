#!/bin/bash
g++ -std=c++11 -Wno-deprecated -o thread-demo threadpool_test.cpp util/YR_ThreadPool.cpp util/YR_Exception.cpp util/YR_TimeProvider.cpp  util/YR_ThreadMutex.cpp util/YR_Thread.cpp util/YR_ThreadCond.cpp util/YR_Common.cpp -I. -lpthread

