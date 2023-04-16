include_directories(${SirusPatcher_SOURCE_DIR}/DBC/inc)

file (GLOB SOURCES *.cc)

add_library(DBC STATIC ${SOURCES})
