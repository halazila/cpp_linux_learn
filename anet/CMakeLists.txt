# CMake 最低版本要求
cmake_minimum_required(VERSION 3.0)

#项目信息
project (tcptest)
include_directories(${CMAKE_CURRENT_LIST_DIR})
aux_source_directory(. ANET_SRC)

# set(CMAKE_CXX_FLAGS "-lpthread")
link_libraries(pthread)

#指定生成目标
add_executable(tcpServer anet.h anet.cpp tcpServer.cpp)
add_executable(tcpClient anet.h anet.cpp tcpClient.cpp)