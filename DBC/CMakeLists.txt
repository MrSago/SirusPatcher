file (GLOB SOURCES *.cc)
add_library(dbc STATIC ${SOURCES})
target_include_directories(dbc PRIVATE ${SirusPatcher_SOURCE_DIR})