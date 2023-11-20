cmake_minimum_required(VERSION 3.20)
project(R3_EDITOR LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_VERSION 6)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

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
        Qt6Core
        Qt6Widgets
)
