set(QT_PREFIX_PATH_CMAKE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/qtprefixpath.cmake)

if (NOT DEFINED CMAKE_PREFIX_PATH AND EXISTS ${QT_PREFIX_PATH_CMAKE_FILE})
  include(${QT_PREFIX_PATH_CMAKE_FILE})
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB TARGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(QtGui
    MANUAL_FINALIZATION
    ${TARGET_SOURCES}
  )
endif()

target_include_directories(QtGui PRIVATE ${SirusPatcher_SOURCE_DIR})
add_dependencies(QtGui stormlib)
target_link_libraries(QtGui PRIVATE StormLib DBC Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(QtGui PROPERTIES
  WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(QtGui)
endif()

find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
add_custom_command(TARGET QtGui POST_BUILD
  COMMAND ${TOOL_WINDEPLOYQT}
          $<TARGET_FILE:QtGui>
  COMMENT "Running windeployqt..."
)

add_custom_command(TARGET QtGui POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${EXTERNAL_DLL_LOCATION}/libStormLib.dll
          $<TARGET_FILE_DIR:QtGui>
  COMMENT "Copying StormLib dlls..."
)
