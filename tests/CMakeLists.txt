cmake_minimum_required(VERSION 3.20)
project(R3_TESTS VERSION 0.0.0 LANGUAGES CXX C)

function(TEST_PROJECT)
	file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
	file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	get_filename_component(DIRNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	add_library(${DIRNAME} SHARED)

	if (MSVC)
		set_target_properties(${DIRNAME} PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
	else ()
		set_target_properties(${DIRNAME} PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
	endif ()

	target_sources(${DIRNAME} PUBLIC ${C_SOURCES} ${CXX_SOURCES})
	target_include_directories(${DIRNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${DIRNAME} PUBLIC R3_ENGINE)
	target_compile_definitions(${DIRNAME} PUBLIC R3_DLL_IMPL=1)
	add_dependencies(R3_APP ${DIRNAME})
	add_dependencies(${DIRNAME} COMPILE_SHADERS)
endfunction()

#[[
file(GLOB SUBDIRS LIST_DIRECTORIES true "*")
foreach (DIR ${SUBDIRS})
	if (IS_DIRECTORY ${DIR})
		add_subdirectory(${DIR})
	endif ()
endforeach ()
]]

add_subdirectory(vulkan)
