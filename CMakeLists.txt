cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable("${CMAKE_PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

if (MSVC)
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS  "${CMAKE_C_FLAGS} /W4"
        )
else ()
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic"
        )
endif ()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    )

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TEST OFF)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glad)
find_package(OpenGL REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    glfw
    glad
    )

