cmake_minimum_required(VERSION 4.3)
project(lib)

set(CMAKE_CXX_STANDARD 20)

add_library(lib SHARED library.cpp)
