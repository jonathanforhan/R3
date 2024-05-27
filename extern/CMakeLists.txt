set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

if (R3_OPENGL)
    find_package(OpenGL REQUIRED)

    # glad
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glad")
    list(APPEND EXTERN_LIBS_PRIVATE "glad")
elseif (R3_VULKAN)
    find_package(Vulkan REQUIRED)
    list(APPEND EXTERN_LIBS_PRIVATE "Vulkan::Vulkan")
else ()
    message(FATAL_ERROR "Renderer must be set [R3_OPENGL|R3_VULKAN]")
endif ()

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4")
list(APPEND EXTERN_LIBS_PRIVATE "glfw")

# glm (public)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glm-1.0.1/glm")
target_include_directories("glm" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm-1.0.1")
list(APPEND EXTERN_LIBS_PUBLIC "glm")

# stb_image
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/stb_image")
list(APPEND EXTERN_LIBS_PRIVATE "stb_image")

# RapidJSON
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_CXX17 ON CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rapidjson-1.1.0")
add_library(RapidJSON_interface INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson-1.1.0/include")
list(APPEND EXTERN_LIBS_PRIVATE "RapidJSON_interface")

# yaml-cpp
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp-0.8.0")
list(APPEND EXTERN_LIBS_PRIVATE "yaml-cpp::yaml-cpp")

# EnTT (public)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/entt-3.13.2")
list(APPEND EXTERN_LIBS_PUBLIC "EnTT::EnTT")

# ImGui
add_library(ImGui SHARED 
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_demo.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_tables.cpp"
	# "$<$<BOOL:R3_VULKAN>:${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/backends/imgui_impl_vulkan.cpp>"
	"$<$<BOOL:R3_OPENGL>:${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/backends/imgui_impl_opengl3.cpp>"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/backends/imgui_impl_glfw.cpp"
)
target_include_directories(ImGui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6")
#target_link_libraries(ImGui PUBLIC "$<$<BOOL:R3_VULKAN>:Vulkan::Vulkan>" "glfw")
if (R3_OPENGL)
	target_link_libraries(ImGui PUBLIC "glfw")
elseif (R3_VULKAN)
	target_link_libraries(ImGui PUBLIC "Vulkan::Vulkan" "glfw")
endif ()
list(APPEND EXTERN_LIBS_PRIVATE "ImGui")

# easy_profiler
set(EASY_PROFILER_NO_GUI ON CACHE BOOL "" FORCE)
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/easy_profiler-2.1.0")
list(APPEND EXTERN_LIBS_PRIVATE "EnTT::EnTT")

# fmt
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/fmt-10.2.1")
list(APPEND EXTERN_LIBS_PUBLIC "fmt::fmt")

set(R3_EXTERN_LIBS_PUBLIC ${EXTERN_LIBS_PUBLIC} PARENT_SCOPE)
set(R3_EXTERN_LIBS_PRIVATE ${EXTERN_LIBS_PRIVATE} PARENT_SCOPE)
