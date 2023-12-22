#include "Editor.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulkan/vulkan.h>
#include "api/Ensure.hpp"
#include "core/Engine.hpp"
#include "render/CommandBuffer.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Queue.hpp"
#include "render/RenderPass.hpp"
#include "render/Window.hpp"

namespace R3::editor {

Editor::Editor() {
    EditorSpecification spec = Engine::renderer().editorRenderInfo();
    m_logicalDevice = spec.logicalDevice;

    VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
    };

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1000,
        .poolSizeCount = (uint32)std::size(poolSizes),
        .pPoolSizes = poolSizes,
    };

    VkDescriptorPool imguiPool;
    auto result =
        vkCreateDescriptorPool(spec.logicalDevice->handle<VkDevice>(), &descriptorPoolInfo, nullptr, &imguiPool);
    ENSURE(result == VK_SUCCESS);

    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForVulkan(spec.window->handle<GLFWwindow*>(), true);

    ImGui_ImplVulkan_InitInfo initInfo = {
        .Instance = spec.instance->handle<VkInstance>(),
        .PhysicalDevice = spec.physicalDevice->handle<VkPhysicalDevice>(),
        .Device = spec.logicalDevice->handle<VkDevice>(),
        .Queue = spec.graphicsQueue->handle<VkQueue>(),
        .DescriptorPool = imguiPool,
        .MinImageCount = spec.minImageCount,
        .ImageCount = spec.imageCount,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
    };

    ImGui_ImplVulkan_Init(&initInfo, spec.renderPass->handle<VkRenderPass>());

    ImGui_ImplVulkan_CreateFontsTexture();

    m_descriptorPool = imguiPool;
}

Editor::~Editor() {
    ImGui_ImplVulkan_DestroyFontsTexture();
    vkDestroyDescriptorPool(m_logicalDevice->as<VkDevice>(), (VkDescriptorPool)m_descriptorPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

void Editor::newFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void Editor::drawUI(Ref<const CommandBuffer> commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer->handle<VkCommandBuffer>());
}

} // namespace R3::editor
