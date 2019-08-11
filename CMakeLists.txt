cmake_minimum_required(VERSION 2.8)
PROJECT(youren)

#设定编译参数


#搜索当前目录下所有源文件并存储在一个变量中
aux_source_directory(./src/log SRC_LIST)

add_executable(test ${SRC_LIST})

set(CMAKE_CXX_FLAGS " -rdynamic -O0 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

