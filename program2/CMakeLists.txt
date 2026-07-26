cmake_minimum_required(VERSION 3.20)
project(program2)

set(CMAKE_CXX_STANDARD 20)

add_executable(program2 main.cpp
        src/SocketServer.cpp
        include/SocketServer.h)

target_link_libraries(program2
        PRIVATE
        lib
)
