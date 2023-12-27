#include "ui/UserInterface.hxx"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulkan/vulkan.hpp>
#include "core/Engine.hpp"
#include "render/CommandBuffer.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Queue.hpp"
#include "render/RenderPass.hpp"
#include "render/Window.hpp"

namespace R3::ui {

UserInterface::UserInterface() {
    vk::DescriptorPoolSize poolSizes[] = {
        {vk::DescriptorType::eSampler, 1000},
        {vk::DescriptorType::eCombinedImageSampler, 1000},
        {vk::DescriptorType::eSampledImage, 1000},
        {vk::DescriptorType::eStorageImage, 1000},
        {vk::DescriptorType::eUniformTexelBuffer, 1000},
        {vk::DescriptorType::eStorageTexelBuffer, 1000},
        {vk::DescriptorType::eUniformBuffer, 1000},
        {vk::DescriptorType::eStorageBuffer, 1000},
        {vk::DescriptorType::eUniformBufferDynamic, 1000},
        {vk::DescriptorType::eStorageBufferDynamic, 1000},
        {vk::DescriptorType::eInputAttachment, 1000},
    };

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {
        .sType = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = 1000,
        .poolSizeCount = (uint32)std::size(poolSizes),
        .pPoolSizes = poolSizes,
    };

    auto& renderer = Engine::renderer();

    m_descriptorPool = Ref<void>(renderer.m_logicalDevice.as<vk::Device>().createDescriptorPool(descriptorPoolInfo));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(renderer.m_window.handle<GLFWwindow*>(), true);

    ImGui_ImplVulkan_InitInfo initInfo = {
        .Instance = renderer.m_instance.as<vk::Instance>(),
        .PhysicalDevice = renderer.m_physicalDevice.as<vk::PhysicalDevice>(),
        .Device = renderer.m_logicalDevice.as<vk::Device>(),
        .Queue = renderer.m_logicalDevice.graphicsQueue().as<vk::Queue>(),
        .DescriptorPool = (VkDescriptorPool)m_descriptorPool.get(),
        .MinImageCount = MAX_FRAMES_IN_FLIGHT,
        .ImageCount = MAX_FRAMES_IN_FLIGHT,
        .MSAASamples = (VkSampleCountFlagBits)renderer.m_physicalDevice.sampleCount(),
    };
    ImGui_ImplVulkan_Init(&initInfo, renderer.m_renderPass.as<vk::RenderPass>());

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("fonts/Cascadia/CascadiaCode.ttf", 16.0f);
    io.Fonts->Build();

    ImGui_ImplVulkan_CreateFontsTexture();
}

UserInterface::~UserInterface() {
    ImGui_ImplVulkan_DestroyFontsTexture();
    Engine::renderer().m_logicalDevice.as<vk::Device>().destroyDescriptorPool((VkDescriptorPool)m_descriptorPool.get());
    ImGui_ImplVulkan_Shutdown();
}

void UserInterface::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::endFrame() {
    ImGui::Render();
}

void UserInterface::draw(const CommandBuffer& commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.as<vk::CommandBuffer>());
}

} // namespace R3::ui
