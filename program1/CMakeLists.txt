add_executable(program1
    main.cpp
    src/IO.cpp
    src/SharedBuffer.cpp
    src/SocketClient.cpp
)

target_include_directories(program1 PRIVATE include)

target_link_libraries(program1
    PRIVATE
        common
        lib
        Threads::Threads
)
