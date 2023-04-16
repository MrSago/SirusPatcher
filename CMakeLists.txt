cmake_minimum_required(VERSION 3.21)
project(SirusPatcher VERSION 0.1 LANGUAGES CXX)

if (NOT WIN32)
  message(FATAL_ERROR "This project is only supported on Windows")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(QtGui)
add_subdirectory(dbc)
