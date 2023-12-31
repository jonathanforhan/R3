#include "UserInterface.hxx"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <R3_components>
#include <R3_core>
#include <format>
#include <vulkan/vulkan.hpp>
#include "render/Renderer.hxx"

namespace R3 {

static constinit auto GUI_BOARDERLESS =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

UserInterface::UserInterface(const UserInterfaceSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
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

    m_descriptorPool = Ref<void>(m_logicalDevice->as<vk::Device>().createDescriptorPool(descriptorPoolInfo));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(spec.window.handle<GLFWwindow*>(), true);

    ImGui_ImplVulkan_InitInfo initInfo = {
        .Instance = spec.instance.as<vk::Instance>(),
        .PhysicalDevice = spec.physicalDevice.as<vk::PhysicalDevice>(),
        .Device = spec.logicalDevice.as<vk::Device>(),
        .Queue = spec.logicalDevice.graphicsQueue().as<vk::Queue>(),
        .DescriptorPool = (VkDescriptorPool)m_descriptorPool.get(),
        .MinImageCount = MAX_FRAMES_IN_FLIGHT,
        .ImageCount = MAX_FRAMES_IN_FLIGHT,
        .MSAASamples = (VkSampleCountFlagBits)spec.physicalDevice.sampleCount(),
    };

    ImGui_ImplVulkan_Init(&initInfo, spec.renderPass.as<vk::RenderPass>());

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("fonts/Cascadia/CascadiaCode.ttf", 16.0f);
    io.Fonts->Build();

    ImGui_ImplVulkan_CreateFontsTexture();
}

UserInterface::~UserInterface() {
    if (m_logicalDevice != nullptr) {
        ImGui_ImplVulkan_DestroyFontsTexture();
        m_logicalDevice->as<vk::Device>().destroyDescriptorPool((VkDescriptorPool)m_descriptorPool.get());
        ImGui_ImplVulkan_Shutdown();
    }
}

void UserInterface::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
}

void UserInterface::endFrame() {
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void UserInterface::drawFrame(const CommandBuffer& commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.as<vk::CommandBuffer>());
}

void UserInterface::displayDeltaTime(double dt) {
    ImGui::Begin("Delta Time", nullptr, GUI_BOARDERLESS);
    ImGui::SetWindowPos(ImVec2(10, 10));
    ImGui::Text("%lf ms", dt * 1000);
    ImGui::End();
}

void UserInterface::lightEditor() {
    static_assert(sizeof(vec3) == 3 * sizeof(float), "need this property for casting");
    uint32 i = 0;

    ImGui::Begin("Light Editor");
    Entity::componentView<LightComponent>().each([&](LightComponent& light) {
        ImGui::PushID(i);
        ImGui::Text("Light %d", i + 1);
        ImGui::DragFloat3("position", glm::value_ptr(light.position), 0.01f);
        ImGui::DragFloat3("color", glm::value_ptr(light.color), 0.005f, 0.0f, 1.0f);
        ImGui::DragFloat("intensity", &light.intensity, 0.005f, 0.0f, 1.0f);
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::PopID();
        i++;
    });
    ImGui::End();
}

void UserInterface::transformEditor() {
    static_assert(sizeof(vec3) == 3 * sizeof(float), "need this property for casting");
    uint32 i = 0;

    ImGui::Begin("Transform Editor");
    Entity::componentView<TransformComponent>().each([&](TransformComponent& transform) {
        ImGui::PushID(i);
        ImGui::Text("Entity %d", i + 1);
        ImGui::DragFloat3("transform", glm::value_ptr(transform[3]), 0.01f);
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::PopID();
        i++;
    });
    ImGui::End();
}

} // namespace R3