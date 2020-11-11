cmake_minimum_required(VERSION 3.0)

project(zmqnet)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(SOURCE
./CommonStruct.h
./ZmqClientEnd.h
./ZmqClientEnd.cpp
./ZmqServerEnd.h
./ZmqServerEnd.cpp
)

include_directories(${CMAKE_CURRENT_LIST_DIR})

# set(CMAKE_CXX_FLAGS "-lpthread")
link_libraries(pthread zmq)
add_executable(client ${SOURCE} ./ClientTest.cpp)
add_executable(server ${SOURCE} ./ServerTest.cpp)