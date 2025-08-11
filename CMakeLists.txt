cmake_minimum_required (VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(R3 LANGUAGES C CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(R3_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}")

################################################################################
# DEPENDENCIES
################################################################################

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable REQUIRED NAMES glslc HINTS Vulkan::glslc)
find_package(vk-bootstrap CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(Tracy CONFIG REQUIRED)

include(FetchContent)

# ImGui docking branch via FetchContent (not available in vcpkg)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY      https://github.com/ocornut/imgui.git
    GIT_TAG             v1.92.1-docking
    GIT_SHALLOW         TRUE
    UPDATE_DISCONNECTED TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(
    imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(
    imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)

################################################################################
# ENGINE
################################################################################

file(GLOB_RECURSE ENGINE_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/engine/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp
)
add_library(R3_ENGINE STATIC ${ENGINE_SRCS})

target_link_libraries(
    R3_ENGINE
	PUBLIC
		glm::glm
		EnTT::EnTT
	PRIVATE
		glfw
		imgui
		Tracy::TracyClient
		Vulkan::Vulkan
		vk-bootstrap::vk-bootstrap
)

target_include_directories(R3_ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/engine/")

target_compile_definitions(
    R3_ENGINE PUBLIC
        $<$<CONFIG:Debug>:R3_DEBUG>
        $<$<CONFIG:Release>:R3_RELEASE>
		$<$<BOOL:${R3_VULKAN}>:R3_VULKAN>
		$<$<BOOL:${R3_OPENGL}>:R3_OPENGL>
		$<$<BOOL:${R3_DIRECTX12}>:R3_DIRECTX12>
		GLM_FORCE_RADIANS=1
		GLM_FORCE_DEPTH_ZERO_TO_ONE=1
)

if (MSVC)
	target_compile_options(
		R3_ENGINE PUBLIC
			$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>
			/Zc:preprocessor
			/EHsc
			/W3
	)
else()
	target_compile_options(
		R3_ENGINE PUBLIC
			-Wall
			-Wextra
			-Wpedantic
			-Wconversion
	)
endif()

################################################################################
# APP
################################################################################

file(GLOB_RECURSE APP_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/app/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp
)
add_executable(R3 ${APP_SRCS})

target_link_libraries(R3 PRIVATE R3_ENGINE)

target_include_directories(R3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app/)

################################################################################
# TESTING (TODO)
################################################################################

enable_testing()

################################################################################
# UTIL
################################################################################

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
		${R3_RUNTIME_OUTPUT_DIRECTORY}/assets
)

find_package(Python3 COMPONENTS Interpreter)

# compile shaders on configure
execute_process(
    COMMAND
		${Python3_EXECUTABLE}
			${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.py
			${glslc_executable}
			spirv-cross
			${CMAKE_CURRENT_SOURCE_DIR}
			${R3_RUNTIME_OUTPUT_DIRECTORY}
)

# compile shaders on build
add_custom_target(COMPILE_SHADERS
    ALL COMMAND
		${Python3_EXECUTABLE}
			${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.py
			${glslc_executable}
			spirv-cross
			${CMAKE_CURRENT_SOURCE_DIR}
			${R3_RUNTIME_OUTPUT_DIRECTORY}
)