cmake_minimum_required(VERSION 3.26)
project(R3_EDITOR LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    Editor.hpp
    Editor.cpp
)

qt_add_executable(R3_EDITOR ${PROJECT_SOURCES})

if (WIN32)
set_target_properties(R3_EDITOR
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)
endif ()

target_link_libraries(R3_EDITOR
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
