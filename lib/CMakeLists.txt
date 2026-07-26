add_library(lib SHARED src/library.cpp)

target_include_directories(lib
    PUBLIC
        include
)
