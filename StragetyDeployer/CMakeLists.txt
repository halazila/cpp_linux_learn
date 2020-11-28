cmake_minimum_required(VERSION 3.0)

project(zmqnet)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(ZMQ_SOURCE
./AsyncZmqApi.h
./AsyncZmqApi.cpp
./CommonStruct.h
./DispatcherManager.h
./DispatcherManager.cpp
./DispatcherService.h
./DispatcherService.cpp
)

set(SQLITE_SOURCE
./SQLiteDatabase.h
./SQLiteDatabase.cpp
./sqlite3ext.h
./sqlite3.h
)

include_directories(${CMAKE_CURRENT_LIST_DIR})
link_directories(.)

set(CMAKE_CXX_FLAGS "-fPIC")
link_libraries(sqlite3 pthread zmq dl)
add_executable(sqlite_test ${SQLITE_SOURCE} ./sqlite_test.cpp)
