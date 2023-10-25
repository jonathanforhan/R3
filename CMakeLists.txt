cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable("${CMAKE_PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

if (MSVC)
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} /W4"
        )
else ()
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic"
        )
endif ()

file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${C_SOURCES} ${CXX_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TEST OFF)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glad)
add_subdirectory(extern/stb)
find_package(OpenGL REQUIRED)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    glfw
    glad
    stb_image
    -static
    -static-libgcc
    -static-libstdc++
    )

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    -DR3_OPENGL

    -DR3_BUILD_DEBUG=1
    -DR3_BUILD_RELEASE=0
    -DR3_BUILD_DIST=0

    -DR3_LOGGING_ENABLED=1
    -DR3_LOG_VERBOSE=1
    -DR3_LOG_INFO=1
    -DR3_LOG_WARNING=1
    -DR3_LOG_ERROR=1

    -DGLM_FORCE_DEPTH_ZERO_TO_ONE=1
    )

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY textures DESTINATION ${CMAKE_BINARY_DIR})

