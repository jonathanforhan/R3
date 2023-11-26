cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 0.0.0 LANGUAGES CXX C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
	add_compile_options("/EHsc")
endif ()

set(R3_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

#[[ RENDERER ]]
option(R3_OPENGL "Use OpenGL Renderer" OFF)
option(R3_VULKAN "Use Vulkan Renderer" OFF)

#[[ BUILD ]]
option(R3_BUILD_DEBUG OFF)
option(R3_BUILD_RELEASE OFF)
option(R3_BUILD_DIST OFF)

#[[ LOGGING ]]
option(R3_LOGGING_ENABLED ON)
option(R3_LOG_VERBOSE ON)
option(R3_LOG_INFO ON)
option(R3_LOG_WARNING ON)
option(R3_LOG_ERROR ON)
option(R3_EXTENDED_RENDER_INFO OFF)

#[[ DOCS ]]
option(R3_DOCS OFF)

#[[ TEST ]]
option(R3_BUILD_TESTS ON)

if (R3_VULKAN)
	find_package(Vulkan REQUIRED COMPONENTS glslc)
	find_program(glslc_executable REQUIRED NAMES glslc HINTS Vulkan::glslc)
endif ()

execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/textures"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures")

execute_process(
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spirv"
)

file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
foreach (SHADER ${SHADERS})
	get_filename_component(FILENAME ${SHADER} NAME)
	add_custom_command(
		COMMAND ${glslc_executable} ${SHADER} -o  "spirv/${FILENAME}.spv"
		OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spirv/${FILENAME}.spv"
		DEPENDS ${SHADER} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spirv"
		COMMENT "Compiling ${FILENAME}"
	)
	list(APPEND SPV_SHADERS  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/spirv/${FILENAME}.spv")
endforeach ()
add_custom_target(COMPILE_SHADERS ALL DEPENDS ${SPV_SHADERS})

add_subdirectory(engine)

#[[
add_subdirectory(editor)
target_sources(R3_EDITOR PRIVATE ${R3_ENGINE_INCLUDES})
target_link_libraries(R3_EDITOR PUBLIC R3_ENGINE)
]]

if(R3_BUILD_TESTS)
	add_subdirectory(tests)
endif()

