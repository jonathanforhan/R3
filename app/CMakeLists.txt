cmake_minimum_required(VERSION 3.20)
project(R3_APP VERSION 0.0.0 LANGUAGES CXX C)

file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")
add_executable(R3_APP ${CXX_SOURCES})

if (MSVC)
	set_target_properties(R3_APP PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
else ()
	set_target_properties(R3_APP PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

target_include_directories(R3_APP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(R3_APP PUBLIC -DR3_APP=1)
target_link_libraries(R3_APP PRIVATE R3_ENGINE)
