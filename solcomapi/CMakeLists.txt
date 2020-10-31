cmake_minimum_required(VERSION 3.0)

project(solcomapi)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")
set(SOURCE
./src/CByteArray.h 
./src/CachedBuffer.h
./src/CommonStruct.h
./src/ISerializable.h
./src/PackageCodec.h
./src/SerializableCache.h
./src/SerializableFactory.h
./src/SillyCommand.h
./src/SolConnClient.cpp
./src/SolConnClient.h
./src/SolConnServer.cpp
./src/SolConnServer.h
./src/SolcomApi.cpp
./src/SolcomApi.h
)

include_directories(${CMAKE_CURRENT_LIST_DIR})

# set(CMAKE_CXX_FLAGS "-lpthread")
link_libraries(pthread uv)
add_executable(client ${SOURCE} ./src/ClientTest.cpp)
add_executable(server ${SOURCE} ./src/ServerTest.cpp)