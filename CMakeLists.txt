cmake_minimum_required(VERSION 3.10)
project(quick_full_duplex)

set(CMAKE_CXX_STANDARD 11)

set(INCLUDE_DIRS
        src/
        src/core/)

set(SRC
        src/main.cpp
        src/core/sockets.cpp
        src/core/sockets.h)

add_executable(quick_full_duplex ${SRC})

target_include_directories(quick_full_duplex PRIVATE ${INCLUDE_DIRS})
