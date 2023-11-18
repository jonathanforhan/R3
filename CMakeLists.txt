cmake_minimum_required(VERSION 3.26)
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

add_custom_target(COPY_SHADERS
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/textures"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures")

add_subdirectory(engine)

add_subdirectory(editor)
target_sources(R3_EDITOR PRIVATE ${R3_ENGINE_INCLUDES})
target_link_libraries(R3_EDITOR PUBLIC R3_ENGINE)

option(R3_BUILD_TESTS ON)
set(R3_BUILD_TESTS ON)
if(R3_BUILD_TESTS)
	add_subdirectory(tests)
endif()
