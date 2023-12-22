find_package(Vulkan REQUIRED)

#[[ IMGUI ]]
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

target_include_directories(ImGui PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../engine/extern/glfw/include"
)

target_link_libraries(ImGui PUBLIC "Vulkan::Vulkan")

list(APPEND EXTERN_LIBS "ImGui")

set(R3_EDITOR_EXTERN_LIBS ${EXTERN_LIBS} PARENT_SCOPE)
