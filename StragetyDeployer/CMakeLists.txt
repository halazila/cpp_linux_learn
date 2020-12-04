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
./DispManagerImpl.h
./DispManagerImpl.cpp
)

set(SQLITE_SOURCE
./SQLiteDatabase.h
./SQLiteDatabase.cpp
./sqlite3ext.h
./sqlite3.h
)

set(DBOP_SOURCE
./dbopmacros.h
./dboperatefunc.h
./dboperatefunc.cpp
)

include_directories(${CMAKE_CURRENT_LIST_DIR})
link_directories(.)

set(CMAKE_CXX_FLAGS "-fPIC")
link_libraries(sqlite3 pthread zmq dl)
add_executable(sqlite_test ${SQLITE_SOURCE} ./sqlite_test.cpp)
add_executable(manager_test ${SQLITE_SOURCE} ${ZMQ_SOURCE} ${DBOP_SOURCE} ./manager_test.cpp)
add_executable(service_test ${SQLITE_SOURCE} ${ZMQ_SOURCE} ${DBOP_SOURCE} ./service_test.cpp)


