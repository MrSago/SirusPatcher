add_executable(Test main.cc)
target_link_libraries(Test dbc)
target_include_directories(Test PRIVATE ${SirusPatcher_SOURCE_DIR})

# add_custom_command(TARGET Test POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Test> $<TARGET_FILE_DIR:Test>
#   COMMAND_EXPAND_LISTS
#   )
