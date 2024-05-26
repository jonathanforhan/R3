project(R3_ENGINE)

file(GLOB_RECURSE CXX_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(R3_ENGINE "${CXX_SRCS}")

if (MSVC)
	if (${CMAKE_BUILD_TYPE} MATCHES "^Debug$")
		target_compile_options(R3_ENGINE PUBLIC "${CMAKE_CXX_FLAGS} /W4 /DEBUG")
	else ()
		target_compile_options(R3_ENGINE PUBLIC "${CMAKE_CXX_FLAGS} /W4")
	endif ()
else ()
	target_compile_options(R3_ENGINE PUBLIC "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

# Renderer Check
set(RENDERER_COUNT 0)
set(RENDERERS R3_OPENGL;R3_VULKAN)
foreach (RENDERER IN LISTS RENDERERS)
	if (${RENDERER})
		math(EXPR RENDERER_COUNT "${RENDERER_COUNT} + 1")
	endif ()
endforeach ()

if (${RENDERER_COUNT} GREATER 1)
	message(FATAL_ERROR "multiple renderers selected")
elseif (NOT ${RENDERER_COUNT})
    message(FATAL_ERROR "renderer must be set")
endif ()

# build check
if (NOT R3_BUILD_DEBUG AND NOT R3_BUILD_RELEASE AND NOT R3_BUILD_DISTRIBUTION)
    message(FATAL_ERROR "build must be set")
elseif ((R3_BUILD_DEBUG AND R3_BUILD_RELEASE) OR (R3_BUILD_DEBUG AND R3_BUILD_DISTRIBUTION) OR (R3_BUILD_RELEASE AND R3_BUILD_DISTRIBUTION))
    message(FATAL_ERROR "too many builds set ")
endif ()


# defines
if (R3_OPENGL)
    find_package(OpenGL REQUIRED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_OPENGL=1)
elseif (R3_VULKAN)
    find_package(Vulkan REQUIRED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_VULKAN=1 -DGLM_FORCE_DEPTH_ZERO_TO_ONE=1)
endif ()

if (R3_BUILD_DEBUG)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_DEBUG=1)
elseif (R3_BUILD_RELEASE)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_RELEASE=1)
elseif (R3_BUILD_DISTRIBUTION)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_DISTRIBUTION=1)
endif ()

if (R3_LOG_VERBOSE)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_VERBOSE=1)
endif ()
if (R3_LOG_INFO)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_INFO=1)
endif ()
if (R3_LOG_WARNING)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_WARNING=1)
endif ()
if (R3_LOG_ERROR)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOG_ERROR=1)
endif ()
if (R3_EDITOR)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_EDITOR=1)
endif ()

target_compile_definitions(R3_ENGINE
	PUBLIC
		-DGLFW_INCLUDE_NONE=1
		-DGLM_FORCE_RADIANS=1
	PRIVATE
		-DR3_ENGINE=1
)

# includes
target_include_directories(R3_ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
