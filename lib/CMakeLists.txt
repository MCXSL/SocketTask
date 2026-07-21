cmake_minimum_required(VERSION 4.3)
project(lib)

set(CMAKE_CXX_STANDARD 20)

add_library(lib SHARED src/library.cpp)

target_include_directories(lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
