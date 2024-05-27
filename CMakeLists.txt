cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 0.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    # MSVC Hot Reload
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif ()

    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_compile_options("/Zc:preprocessor")
endif ()

# Renderer
option(R3_OPENGL "Use OpenGL Renderer" OFF)
option(R3_VULKAN "Use VULKAN Renderer" OFF)

# Build
option(R3_BUILD_DEBUG OFF)
option(R3_BUILD_RELEASE OFF)
option(R3_BUILD_DISTRIBUTION OFF)

# Editor
option(R3_EDITOR OFF)

# Logging
option(R3_LOG_VERBOSE ON)
option(R3_LOG_INFO ON)
option(R3_LOG_WARNING ON)
option(R3_LOG_ERROR ON)

# Tests
option(R3_TESTS OFF)

# Assets and Shaders
if (R3_VULKAN)
    find_package(Vulkan REQUIRED COMPONENTS glslc)
    find_program(glslc_executable REQUIRED NAMES glslc HINTS Vulkan::glslc)
endif ()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures"
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fonts"
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spirv"
)

find_package(Python3 COMPONENTS Interpreter)

add_custom_target(COMPILE_SHADERS ALL COMMAND
    "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.py"
        "${glslc_executable}"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spirv"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/extern")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/engine")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/app")
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")

target_link_libraries(R3_ENGINE
    PRIVATE
        "${R3_EXTERN_LIBS_PRIVATE}"
    PUBLIC
        "${R3_EXTERN_LIBS_PUBLIC}"
)
target_link_libraries(R3_APP PRIVATE R3_ENGINE)

#[[ DOCUMENTATION ]]
if (R3_DOCS)
    find_package(Doxygen)

    if (DOXYGEN_FOUND)
        execute_process(
            COMMAND ${DOXYGEN_EXECUTABLE} "${R3_SOURCE_DIR}/Doxyfile"
                WORKING_DIRECTORY ${R3_SOURCE_DIR}
                OUTPUT_FILE "${R3_SOURCE_DIR}/docs/doxygen.log.txt"
                ERROR_FILE "${R3_SOURCE_DIR}/docs/doxygen.log.txt"
        )
    endif ()
endif ()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/compile_commands.json"
        "${R3_SOURCE_DIR}/compile_commands.json"
)