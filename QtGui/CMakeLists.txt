set(CMAKE_PREFIX_PATH "C:/Qt/6.3.1/msvc2019_64")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        siruspatcherwindow.cpp
        siruspatcherwindow.h
        siruspatcherwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QtGui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
endif()

target_include_directories(QtGui PRIVATE ${SirusPatcher_SOURCE_DIR})

target_link_libraries(QtGui PRIVATE dbc Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(QtGui PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtGui)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_program(TOOL_WINDEPLOYQT NAMES windeployqt.debug.bat)
else()
	find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
endif()

add_custom_command(TARGET QtGui POST_BUILD
    COMMAND ${TOOL_WINDEPLOYQT}
            $<TARGET_FILE:QtGui>
    COMMENT "Running windeployqt..."
)
