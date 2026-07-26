add_library(common STATIC
        src/Logger.cpp
)

target_include_directories(common PUBLIC
        include
)