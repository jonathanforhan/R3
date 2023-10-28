cmake_minimum_required(VERSION 3.20)
project(R3_ENGINE VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE H_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE HXX_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

list(APPEND R3_ENGINE_SOURCES ${C_SOURCES} ${CXX_SOURCES})
list(APPEND R3_ENGINE_HEADERS ${H_HEADERS} ${HXX_HEADERS})
set(R3_ENGINE_INCLUDES ${R3_ENGINE_HEADERS} PARENT_SCOPE)

add_library(R3_ENGINE STATIC ${R3_ENGINE_SOURCES})
target_include_directories(R3_ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (MSVC)
    set_target_properties(R3_ENGINE PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
else ()
    set_target_properties(R3_ENGINE PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif ()

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TEST OFF)
add_subdirectory(extern/glad)
target_compile_options(glad PUBLIC "-w")
add_subdirectory(extern/glfw)
target_compile_options(glfw PUBLIC "-w")
add_subdirectory(extern/glm)
target_compile_options(glm INTERFACE "-w")
add_subdirectory(extern/stb)
target_compile_options(stb_image INTERFACE "-w")
find_package(OpenGL REQUIRED)

include(GNUInstallDirs)

target_link_libraries(R3_ENGINE PUBLIC
    glad
    glfw
    glm
    stb_image
    )

target_compile_definitions(R3_ENGINE PUBLIC
    -DR3_OPENGL=1

    -DR3_BUILD_DEBUG=1
    -DR3_BUILD_RELEASE=0
    -DR3_BUILD_DIST=0

    -DR3_LOGGING_ENABLED=1
    -DR3_LOG_VERBOSE=1
    -DR3_LOG_INFO=1
    -DR3_LOG_WARNING=1
    -DR3_LOG_ERROR=1

    # -DGLM_FORCE_DEPTH_ZERO_TO_ONE=1
    -DGLFW_INCLUDE_NONE=1
    -DGLFW_EXPOSE_NATIVE_WIN32=1
    # -DGLFW_EXPOSE_NATIVE_X11=1
    )
