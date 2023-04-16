cmake_minimum_required(VERSION 3.21)
project(SirusPatcher LANGUAGES C CXX)

if (NOT WIN32)
    message(FATAL_ERROR "This project is only supported on Windows")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Test)
add_subdirectory(dbc)
