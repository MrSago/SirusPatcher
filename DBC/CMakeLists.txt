file(GLOB TARGET_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)
add_library(DBC STATIC ${TARGET_SOURCES})
target_include_directories(DBC PRIVATE ${SirusPatcher_SOURCE_DIR})