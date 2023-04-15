include_directories(${SirusPatcher_SOURCE_DIR}/Test/inc)
include_directories(${SirusPatcher_SOURCE_DIR}/DBC/inc)

# link_directories(${SirusPatcher_SOURCE_DIR}/DBC/src)

add_executable(Test main.cc)
target_link_libraries(Test DBC)
add_custom_command(TARGET Test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Test> $<TARGET_FILE_DIR:Test>
  COMMAND_EXPAND_LISTS
  )