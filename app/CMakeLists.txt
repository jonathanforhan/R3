project(R3_APP)

file(GLOB_RECURSE CXX_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")

if (WIN32 AND R3_BUILD_DISTRIBUTION)
	add_executable(R3_APP WIN32 "${CXX_SRCS}")
else ()
	add_executable(R3_APP "${CXX_SRCS}")
endif ()

if (MSVC)
	if (${CMAKE_BUILD_TYPE} MATCHES "^Release$")
		target_compile_options(R3_APP PUBLIC "${CMAKE_CXX_FLAGS} /W4")
	else ()
		target_compile_options(R3_APP PUBLIC "${CMAKE_CXX_FLAGS} /W4 /Od")
	endif ()
else ()
	target_compile_options(R3_APP PUBLIC "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

target_include_directories(R3_APP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(R3_APP PUBLIC -DR3_APP=1)
