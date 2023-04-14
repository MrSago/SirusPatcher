include_directories(${SirusPatcher_SOURCE_DIR}/DBCLib/inc)

file (GLOB SOURCES *.cc)

add_library(DBC SHARED ${SOURCES})