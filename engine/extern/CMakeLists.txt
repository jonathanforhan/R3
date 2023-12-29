#[=[ R3 Extern Dependencies ]=]

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (R3_OPENGL)
	#[[ OpenGL ]]
	find_package(OpenGL REQUIRED)
elseif (R3_VULKAN)
	#[[ Vulkan ]]
	find_package(Vulkan REQUIRED)
	list(APPEND EXTERN_LIBS_PRIVATE "Vulkan::Vulkan")
else ()
	message(FATAL_ERROR "Renderer must be set")
endif ()

include(GNUInstallDirs)

if (R3_OPENGL)
#[[ Glad ]]
add_subdirectory(glad SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "glad")
endif ()

#[[ GLFW ]]
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "glfw")

#[[ glm ]]
add_subdirectory(glm SYSTEM)
list(APPEND EXTERN_LIBS "glm")

#[[ stb_image ]]
add_subdirectory(stb SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "stb_image")

#[[ EnTT ]]
add_subdirectory(entt SYSTEM)
list(APPEND EXTERN_LIBS "EnTT::EnTT")

#[[ ImGui ]]
find_package(Vulkan REQUIRED)
add_library(ImGui STATIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
)
target_include_directories(ImGui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
target_link_libraries(ImGui PUBLIC "Vulkan::Vulkan" "glfw")
list(APPEND EXTERN_LIBS_PRIVATE "ImGui")

#[[ RapidJSON ]]
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX17 ON CACHE BOOL "" FORCE)
add_subdirectory(rapidjson SYSTEM)
list(APPEND EXTERN_INCLUDE_DIRS_PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")

#[[ Optick ]]
if (NOT R3_BUILD_DIST AND MSVC)
add_subdirectory(optick)
list(APPEND EXTERN_LIBS_PRIVATE "OptickCore")
endif ()

set(R3_EXTERN_LIBS ${EXTERN_LIBS} PARENT_SCOPE)
set(R3_EXTERN_INCLUDE_DIRS ${EXTERN_INCLUDE_DIRS} PARENT_SCOPE)

set(R3_EXTERN_LIBS_PRIVATE ${EXTERN_LIBS_PRIVATE} PARENT_SCOPE)
set(R3_EXTERN_INCLUDE_DIRS_PRIVATE ${EXTERN_INCLUDE_DIRS_PRIVATE} PARENT_SCOPE)
