cmake_minimum_required(VERSION 3.10)
project(network_simplex)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(common network test)


set(SRC_FILES
        common/common.h
        common/hash_type.h
        common/utils.h
        common/utils.cpp
        network/node.h
        network/node.cpp
        network/arc.h
        network/arc.cpp
        network/network.h
        network/network.cpp
        network/network_utils.h
        network/network_utils.cpp)

add_executable(test_main
        main.cpp)

add_executable(test_utils
        test/test_utils.cpp
        common/utils.h
        common/utils.cpp
        )

add_executable(test_network
        test/test_network.cpp
        ${SRC_FILES})
