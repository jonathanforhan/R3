cmake_minimum_required(VERSION 3.20)
project(R3 VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
file(GLOB TEXTURE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/textures/*")
list(APPEND SHADERS ${SHADER_FILES})
list(APPEND TEXTURES ${TEXTURE_FILES})

macro(COPY_RESOURCES DIR)
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${DIR}/shaders")
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${DIR}/textures")
	foreach(FILE ${SHADERS})
		get_filename_component(FILE_NAME ${FILE} NAME)
		configure_file(
			"${CMAKE_CURRENT_SOURCE_DIR}/shaders/${FILE_NAME}"
			"${CMAKE_BINARY_DIR}/${DIR}/shaders/${FILE_NAME}"
			COPYONLY)
	endforeach()
	foreach(FILE ${TEXTURES})
		get_filename_component(FILE_NAME ${FILE} NAME)
		configure_file(
			"${CMAKE_CURRENT_SOURCE_DIR}/textures/${FILE_NAME}"
			"${CMAKE_BINARY_DIR}/${DIR}/textures/${FILE_NAME}"
			COPYONLY)
	endforeach()
endmacro()

option(R3_BUILD_TESTS ON)
set(R3_BUILD_TESTS ON)

add_subdirectory(engine)

add_subdirectory(editor)
target_sources(R3_EDITOR PRIVATE ${R3_ENGINE_INCLUDES})
target_link_libraries(R3_EDITOR PUBLIC R3_ENGINE)
COPY_RESOURCES("editor")

if(R3_BUILD_TESTS)
	add_subdirectory(tests)
	COPY_RESOURCES("tests")
endif()
