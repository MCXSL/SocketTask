add_executable(program2
    main.cpp
    src/SocketServer.cpp
)

target_include_directories(program2 PRIVATE include)

target_link_libraries(program2
    PRIVATE
        common
        lib
)
