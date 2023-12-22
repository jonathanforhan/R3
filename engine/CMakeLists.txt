cmake_minimum_required(VERSION 3.20)
project(R3_ENGINE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CXX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cxx")

# [[ EXCLUDE RENDERERS NOT IN USE ]]
if (NOT R3_OPENGL)
	list(FILTER CXX_SOURCES EXCLUDE REGEX "/opengl-[a-z|A-Z|0-9]*\\.cxx")
endif ()
if (NOT R3_VULKAN)
	list(FILTER CXX_SOURCES EXCLUDE REGEX "/vulkan-[a-z|A-Z|0-9]*\\.cxx")
endif ()
if (NOT R3_DX12)
	list(FILTER CXX_SOURCES EXCLUDE REGEX "/dx12-[a-z|A-Z|0-9]*\\.cxx")
endif ()
list(APPEND R3_ENGINE_SOURCES ${CXX_SOURCES})

#[[ R3 ENGINE ]]
add_library(R3_ENGINE STATIC ${R3_ENGINE_SOURCES})

if (MSVC)
	set_target_properties(R3_ENGINE PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
	foreach (VKCPP IN LISTS ${VULKAN_SOURCES})
		set_source_files_properties(${VKCPP} PROPERTIES COMPILE_FLAGS "/Yuvulkan.hpp")
	endforeach ()
else ()
	set_target_properties(R3_ENGINE PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

file(GLOB PRE_COMPILE_API "${CMAKE_CURRENT_SOURCE_DIR}/public/api/*.hpp")
target_precompile_headers(R3_ENGINE PUBLIC
	"${PRE_COMPILE_API}"
	"public/render/DeviceMemory.hpp"
	"public/render/NativeRenderObject.hpp"
	"public/render/LogicalDevice.hpp"
)

#[[ SANITY CHECK ]]
set(RENDERER_COUNT 0)
set(RENDERERS R3_OPENGL;R3_VULKAN;R3_DX12)
foreach (RENDERER IN LISTS RENDERERS)
	if (${RENDERER})
		math(EXPR RENDERER_COUNT "${RENDERER_COUNT} + 1")
	endif ()
endforeach ()

if (${RENDERER_COUNT} GREATER 1)
	message(FATAL_ERROR "Multiple renderers selected")
elseif (NOT ${RENDERER_COUNT})
    message(FATAL_ERROR "Renderer must be set")
endif ()

if (NOT R3_BUILD_DEBUG AND NOT R3_BUILD_RELEASE AND NOT R3_BUILD_DIST)
    message(FATAL_ERROR "Build must be set")
elseif ((R3_BUILD_DEBUG AND R3_BUILD_RELEASE) OR (R3_BUILD_DEBUG AND R3_BUILD_DIST) OR (R3_BUILD_RELEASE AND R3_BUILD_DIST))
    message(FATAL_ERROR "Build option error")
endif ()

#[[ R3 DEFINES ]]
if (R3_OPENGL)
    find_package(OpenGL REQUIRED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_OPENGL=1)
elseif (R3_VULKAN)
    find_package(Vulkan REQUIRED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_VULKAN=1 -DGLM_FORCE_DEPTH_ZERO_TO_ONE=1)
elseif (R3_DX12)
    message(FATAL_ERROR "DX12 not currently supported")
endif ()

if (R3_BUILD_DEBUG)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_DEBUG=1)
elseif (R3_BUILD_RELEASE)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_RELEASE=1)
elseif (R3_BUILD_DIST)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_BUILD_DIST=1)
endif ()

if (R3_LOGGING_ENABLED)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_LOGGING_ENABLED=1)
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
if (R3_EXTENDED_RENDER_INFO)
	target_compile_definitions(R3_ENGINE PUBLIC -DR3_EXTENDED_RENDER_INFO=1)
endif ()

target_compile_definitions(R3_ENGINE PUBLIC
	-DGLFW_INCLUDE_NONE=1
	-DGLM_FORCE_RADIANS=1
	-DVULKAN_HPP_PRECOMPILE=1
	-DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
	-DVULKAN_HPP_NO_SMART_HANDLE=1
	-DVULKAN_HPP_NO_STRUCT_SETTERS=1
	-DVULKAN_HPP_NO_SPACESHIP_OPERATOR=1
	-DVULKAN_HPP_NO_TO_STRING=1
)

if (WIN32)
    target_compile_definitions(R3_ENGINE PUBLIC -DGLFW_EXPOSE_NATIVE_WIN32=1)
else ()
    target_compile_definitions(R3_ENGINE PUBLIC -DGLFW_EXPOSE_NATIVE_X11=1)
endif ()

#[[ R3 INCLUDES ]]
target_include_directories(R3_ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(R3_ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(R3_ENGINE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

#[[ R3 EXTERNAL DEPENDECIES ]]
add_subdirectory(extern)

target_link_libraries(R3_ENGINE PUBLIC ${R3_EXTERN_LIBS})
target_link_libraries(R3_ENGINE PRIVATE ${R3_EXTERN_LIBS_PRIVATE})

target_include_directories(R3_ENGINE PUBLIC "${R3_EXTERN_INCLUDE_DIRS}")
target_include_directories(R3_ENGINE PRIVATE "${R3_EXTERN_INCLUDE_DIRS_PRIVATE}")

#[[ DOCUMENTATION ]]
if (R3_DOCS)
	find_package(Doxygen)

	if (DOXYGEN_FOUND)
		add_custom_target(R3_DOCS
			COMMAND ${DOXYGEN_EXECUTABLE} "${R3_SOURCE_DIR}/Doxyfile"
			WORKING_DIRECTORY ${R3_SOURCE_DIR}
			COMMENT "Generating R3 Documentation with Doxygen"
			OUTPUT_QUIET
		)
		add_dependencies(R3_ENGINE R3_DOCS)
	endif ()
endif ()
