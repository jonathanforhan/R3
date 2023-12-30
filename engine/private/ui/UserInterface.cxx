#include "UserInterface.hxx"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <R3_core>
#include <vulkan/vulkan.hpp>
#include "render/Renderer.hxx"

namespace R3 {

void UserInterface::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::endFrame() {
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void UserInterface::populate() {
    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::ShowDemoWindow();
}

void UserInterface::drawFrame(const CommandBuffer& commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.as<vk::CommandBuffer>());
}

} // namespace R3