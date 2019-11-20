cmake_minimum_required(VERSION 3.10)
project(network_simplex)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -O3 -ffast-math -finline-functions")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(common network simplex)

set(SRC_FILES
        common/common.h
        common/hash_type.h
        common/utils.cpp
        common/utils.h
        network/node.cpp
        network/node.h
        network/arc.cpp
        network/arc.h
        network/network.cpp
        network/network.h
        network/network_utils.cpp
        network/network_utils.h
        simplex/cycle.cpp
        simplex/cycle.h
        simplex/tree.cpp
        simplex/tree.h)

add_library(netflow
        ${SRC_FILES})

link_libraries(netflow)

add_executable(cnet main.cpp)

add_executable(test_utils test/test_utils.cpp)

add_executable(test_network test/test_network.cpp)

add_executable(test_tree test/test_tree.cpp)
