cmake_minimum_required(VERSION 3.21)
project(SirusPatcher VERSION 0.1 LANGUAGES CXX)

if (NOT WIN32)
  message(FATAL_ERROR "This project is only supported on Windows")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set(EXTERNAL_DLL_LOCATION ${EXTERNAL_INSTALL_LOCATION}/bin)

ExternalProject_Add(stormlib
  GIT_REPOSITORY https://github.com/ladislav-zezula/StormLib
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION} -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

include_directories(${CMAKE_BINARY_DIR})
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

add_subdirectory(DBC)
add_subdirectory(QtGui)
