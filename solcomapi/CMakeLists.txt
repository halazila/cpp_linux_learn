cmake_minimum_required(VERSION 3.0)

project(solcomapi)
set(CMAKE_CXX_STANDARD 11)

aux_source_directory(src SRC_LIST)
include_directories(${CMAKE_CURRENT_LIST_DIR})

# set(CMAKE_CXX_FLAGS "-lpthread")
link_libraries(pthread uv)
add_executable(solclient ${SRC_LIST})