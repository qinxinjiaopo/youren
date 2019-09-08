cmake_minimum_required(VERSION 2.8)
PROJECT(youren)

#设定编译参数


#搜索当前目录下所有源文件并存储在一个变量中
aux_source_directory(. SRC_LIST)

add_subdirectory(util)

add_executable(log-demo ${SRC_LIST})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O2 -Wall -Wno-deprecated -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -Wno-deprecated -pthread")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(log-demo yourenutil)

