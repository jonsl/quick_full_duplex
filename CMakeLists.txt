cmake_minimum_required(VERSION 3.10)
project(quick_full_duplex)

set(CMAKE_CXX_STANDARD 11)

set(INCLUDE_DIRS
        src/
        src/core/)

set(SRC
        src/core/config.h
        src/core/error.h
        src/core/procinfo.h
        src/core/proctype.h
        src/core/process.h
        src/core/socket_type.h
        src/core/sockets.c
        src/core/sockets.h
        src/core/worker.h
        src/main.cpp)

add_executable(quick_full_duplex ${SRC})

target_include_directories(quick_full_duplex PRIVATE ${INCLUDE_DIRS})
