cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 1.0.0 LANGUAGES CXX C)

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

add_custom_target(COPY_ASSETS
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
add_custom_target(COPY_SHADERS
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")
add_custom_target(COPY_TEXTURES
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/textures"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures")

function(TEST_PROJECT)
	file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
	file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	get_filename_component(DIRNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	add_executable(${DIRNAME})

	if (MSVC)
		set_target_properties(${DIRNAME} PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
	else ()
		set_target_properties(${DIRNAME} PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
	endif ()

	target_sources(${DIRNAME} PUBLIC ${C_SOURCES} ${CXX_SOURCES})
	target_include_directories(${DIRNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${DIRNAME} PUBLIC R3_ENGINE)
	add_dependencies(${DIRNAME} COPY_ASSETS)
	add_dependencies(${DIRNAME} COPY_SHADERS)
	add_dependencies(${DIRNAME} COPY_TEXTURES)
endfunction()


option(R3_BUILD_TESTS ON)
set(R3_BUILD_TESTS ON)

add_subdirectory(engine)

add_subdirectory(editor)
target_sources(R3_EDITOR PRIVATE ${R3_ENGINE_INCLUDES})
target_link_libraries(R3_EDITOR PUBLIC R3_ENGINE)

if(R3_BUILD_TESTS)
	add_subdirectory(tests)
endif()
