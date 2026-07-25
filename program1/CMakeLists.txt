cmake_minimum_required(VERSION 4.3)
project(program1)

set(CMAKE_CXX_STANDARD 20)

add_executable(program1 main.cpp
        src/IO.cpp
        include/IO.h
        src/SharedBuffer.cpp
        include/SharedBuffer.h)

target_link_libraries(program1
        PRIVATE
        lib
)

# Copy shared library to exe directory after build (Windows & Linux)
add_custom_command(TARGET program1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:lib>
        $<TARGET_FILE_DIR:program1>/
        COMMENT "Copying shared library to program1 directory"
)

# On Linux, also set RPATH for better compatibility
if(UNIX AND NOT APPLE)
        set_target_properties(program1 PROPERTIES
                BUILD_RPATH "$<TARGET_FILE_DIR:lib>"
                INSTALL_RPATH "$ORIGIN"
        )
endif()
