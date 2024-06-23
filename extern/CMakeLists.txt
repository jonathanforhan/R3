set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

if (R3_OPENGL)
    # OpenGL
    find_package(OpenGL REQUIRED)

    # glad
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glad" SYSTEM)
    list(APPEND EXTERN_LIBS_PRIVATE "glad")
elseif (R3_VULKAN)
    # Vulkan
    find_package(Vulkan REQUIRED)
    list(APPEND EXTERN_LIBS_PRIVATE "Vulkan::Vulkan")
else ()
    message(FATAL_ERROR "Renderer must be set [R3_OPENGL|R3_VULKAN]")
endif ()

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4" SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "glfw")

# glm (public)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glm-1.0.1/glm" SYSTEM)
target_include_directories("glm" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm-1.0.1")
list(APPEND EXTERN_LIBS_PUBLIC "glm")

# stb_image
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/stb_image" SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "stb_image")

# nlohmann json
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Diagnostics $<BOOL:${R3_BUILD_DEBUG}> CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/json-3.11.3" SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "nlohmann_json::nlohmann_json")

# EnTT (public)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/entt-3.13.2" SYSTEM)
list(APPEND EXTERN_LIBS_PUBLIC "EnTT::EnTT")

# ImGui
add_library(ImGui SHARED 
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/imgui_tables.cpp"
    "$<$<BOOL:${R3_VULKAN}>:${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/backends/imgui_impl_vulkan.cpp>"
    "$<$<BOOL:${R3_OPENGL}>:${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/backends/imgui_impl_opengl3.cpp>"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6/backends/imgui_impl_glfw.cpp"
)
target_include_directories(ImGui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui-1.90.6")
target_link_libraries(ImGui PUBLIC "$<$<BOOL:${R3_VULKAN}>:Vulkan::Vulkan>" "glfw")
if (R3_OPENGL)
    target_link_libraries(ImGui PUBLIC "glfw")
elseif (R3_VULKAN)
    target_link_libraries(ImGui PUBLIC "Vulkan::Vulkan" "glfw")
endif ()
list(APPEND EXTERN_LIBS_PRIVATE "ImGui")

# easy_profiler
set(EASY_PROFILER_NO_GUI ON CACHE BOOL "" FORCE)
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/easy_profiler-2.1.0" SYSTEM)
list(APPEND EXTERN_LIBS_PRIVATE "easy_profiler")

# fmt
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/fmt-10.2.1")
list(APPEND EXTERN_LIBS_PUBLIC "fmt::fmt")

set(R3_EXTERN_LIBS_PUBLIC ${EXTERN_LIBS_PUBLIC} PARENT_SCOPE)
set(R3_EXTERN_LIBS_PRIVATE ${EXTERN_LIBS_PRIVATE} PARENT_SCOPE)
