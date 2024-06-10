project(R3_ENGINE)

file(GLOB_RECURSE CXX_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")
if (NOT R3_OPENGL)
	list(FILTER CXX_SOURCES EXCLUDE REGEX "/opengl-[a-z|A-Z|0-9]*\\.cxx")
endif ()
if (NOT R3_VULKAN)
	list(FILTER CXX_SOURCES EXCLUDE REGEX "/vulkan-[a-z|A-Z|0-9]*\\.cxx")
endif ()

add_library(R3_ENGINE "${CXX_SRCS}")

if (MSVC)
	if (${CMAKE_BUILD_TYPE} MATCHES "^Release$")
		target_compile_options(R3_ENGINE PUBLIC "${CMAKE_CXX_FLAGS} /W4")
	else ()
		target_compile_options(R3_ENGINE PUBLIC "${CMAKE_CXX_FLAGS} /W4 /Od")
	endif ()
else ()
	target_compile_options(R3_ENGINE PUBLIC "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

# defines
if (R3_OPENGL)
    find_package(OpenGL REQUIRED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_OPENGL=1)
elseif (R3_VULKAN)
    find_package(Vulkan REQUIRED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_VULKAN=1 -DGLM_FORCE_DEPTH_ZERO_TO_ONE=1)
endif ()

target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_DEBUG=$<BOOL:${R3_BUILD_DEBUG}>)
target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_RELEASE=$<BOOL:${R3_BUILD_RELEASE}>)
target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_DISTRIBUTION=$<BOOL:${R3_BUILD_DISTRIBUTION}>)

target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_VERBOSE=$<BOOL:${R3_LOG_VERBOSE}>)
target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_INFO=$<BOOL:${R3_LOG_INFO}>)
target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_WARNING=$<BOOL:${R3_LOG_WARNING}>)
target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_ERROR=$<BOOL:${R3_LOG_ERROR}>)
target_compile_definitions(R3_ENGINE PUBLIC -DR3_EDITOR=$<BOOL:${R3_EDITOR}>)

target_compile_definitions(R3_ENGINE
	PUBLIC
		-DGLFW_INCLUDE_NONE=1
		-DGLM_FORCE_RADIANS=1
	PRIVATE
		-DR3_ENGINE=1
)

# includes
target_include_directories(R3_ENGINE
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/public"
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/private"
)
