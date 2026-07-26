add_library(common STATIC
    src/CommandLine.cpp
    src/Logger.cpp
)

target_include_directories(common PUBLIC
    include
)
