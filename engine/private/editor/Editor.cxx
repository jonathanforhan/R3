#include "editor/Editor.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <R3_core>
#include <R3_input>
#include <filesystem>
#include <glm/gtx/quaternion.hpp>
#include <vulkan/vulkan.hpp>
#include "components/EditorComponent.hpp"

namespace R3::editor {

static constexpr auto GUI_BOARDERLESS =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

Editor::Editor(const EditorSpecification& spec)
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

    // setup fonts
    io.Fonts->AddFontFromFileTTF("fonts/Roboto/Roboto-Medium.ttf", 16.5f);
    io.Fonts->Build();

    ImGui_ImplVulkan_CreateFontsTexture();
}

Editor::~Editor() {
    if (m_logicalDevice != nullptr) {
        ImGui_ImplVulkan_DestroyFontsTexture();
        m_logicalDevice->as<vk::Device>().destroyDescriptorPool((VkDescriptorPool)m_descriptorPool.get());
        ImGui_ImplVulkan_Shutdown();
    }
}

void Editor::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto& io = ImGui::GetIO();
    float scaleFactor = io.DisplayFramebufferScale.x;
    ImGui::GetStyle().ScaleAllSizes(scaleFactor);

    ImGui::ShowDemoWindow();
}

void Editor::endFrame() {
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void Editor::drawFrame(const CommandBuffer& commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.as<vk::CommandBuffer>());
}

void Editor::setContentScale(float scale) {
    ImGui::GetIO().FontGlobalScale = scale;
}

void Editor::displayDeltaTime(double dt) {
    ImGui::Begin("Delta Time", nullptr, GUI_BOARDERLESS);
    ImGui::SetWindowPos(ImVec2(10, 10));
    ImGui::Text("%lf ms", dt * 1000.0f);
    ImGui::End();
}

void Editor::initializeDocking() {
    static constexpr ImGuiDockNodeFlags dockspaceFlags =
        ImGuiDockNodeFlags_PassthruCentralNode | (int)ImGuiDockNodeFlags_NoWindowMenuButton;
    ImGui::DockSpaceOverViewport(nullptr, dockspaceFlags);
}

void Editor::displayHierarchy() {
    // Hierarchy Panel
    if (ImGui::Begin("Hierarchy")) {
        // Tree Nodes for Scenes
        if (ImGui::TreeNodeEx(CurrentScene->name)) {
            // The Scene's Entities
            Entity::forEach([this](EntityView& entity) {
                if (auto* editorComponent = entity.tryGet<EditorComponent>()) {
                    CHECK(editorComponent->name != nullptr);
                    if (ImGui::Button(editorComponent->name)) {
                        m_currentEntity = entity.id();
                    }
                }
            });
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

void Editor::displayProperties() {
    if (ImGui::Begin("Properties")) {
        if (m_currentEntity != undefined) {
            EntityView entityView(uuid32(m_currentEntity), CurrentScene);

            auto* maybeEditorComponent = entityView.tryGet<EditorComponent>();
            if (!maybeEditorComponent) {
                ImGui::End();
                return;
            }
            auto& editorComponent = *maybeEditorComponent;

            ImGui::Text("%s", editorComponent.name);

            const float sensitivity = ImGui::GetIO().KeyShift ? 0.0001f : 0.1f;
            const bool scaleLock = ImGui::GetIO().KeyCtrl;
            const auto sliderFlags = scaleLock ? ImGuiSliderFlags_NoInput : ImGuiSliderFlags_None;

            // Edit position
            vec3 position = editorComponent.position;
            ImGui::DragFloat3("position", glm::value_ptr(position), sensitivity, 0, 0, "%.4f", sliderFlags);
            vec3 deltaPosition = position - editorComponent.position;
            editorComponent.position = position;
            ImGui::Spacing();

            // Edit Rotation
            vec3 rotation = editorComponent.rotation;
            ImGui::DragFloat3("rotation", glm::value_ptr(rotation), sensitivity, 0, 0, "%.4f", sliderFlags);
            vec3 deltaRotation = rotation - editorComponent.rotation;
            rotation.x = rotation.x < 0 ? 360 : rotation.x >= 360 ? 0 : rotation.x;
            rotation.y = rotation.y < 0 ? 360 : rotation.y >= 360 ? 0 : rotation.y;
            rotation.z = rotation.z < 0 ? 360 : rotation.z >= 360 ? 0 : rotation.z;
            editorComponent.rotation = rotation;
            mat4 rotationMatrix = mat4(1.0f);
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(deltaRotation.x), vec3(1, 0, 0));
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(deltaRotation.y), vec3(0, 1, 0));
            rotationMatrix = glm::rotate(rotationMatrix, glm::radians(deltaRotation.z), vec3(0, 0, 1));
            ImGui::Spacing();

            // Edit scale
            auto scale = editorComponent.scale;
            ImGui::DragFloat3("scale", glm::value_ptr(scale), sensitivity, 0, 0, "%.4f", sliderFlags);
            if (scaleLock) {
                vec3 dscale = scale - editorComponent.scale;
                // two of the components will be zero so we can sum them, no need to check
                float delta = dscale.x + dscale.y + dscale.z;
                scale = editorComponent.scale + vec3(delta);
            }
            scale.x = scale.x <= 0 ? 0.0001f : scale.x;
            scale.y = scale.y <= 0 ? 0.0001f : scale.y;
            scale.z = scale.z <= 0 ? 0.0001f : scale.z;
            vec3 deltaScale = scale / editorComponent.scale;
            editorComponent.scale = scale;
            ImGui::Spacing();

            mat4& transform = entityView.get<TransformComponent>();
            transform = transform * rotationMatrix;
            transform[3] += vec4(deltaPosition, 0.0f);
            transform = glm::scale(transform, deltaScale);
        }
    }
    ImGui::End();
}

void Editor::displaySceneManager() {
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    static constexpr ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH |
                                             ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
                                             ImGuiTableFlags_NoBordersInBody;

    static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAllColumns;

    ImGui::Begin("Scene Manager");
    if (ImGui::BeginTable("3ways", 3, flags)) {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is
        // On
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
        ImGui::TableHeadersRow();

        static void (*fileTree)(const std::filesystem::directory_entry) = [](const auto fd) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (std::filesystem::is_directory(fd)) {
                const bool open = ImGui::TreeNodeEx(fd.path().filename().c_str(), treeNodeFlags);
                ImGui::TableNextColumn();
                ImGui::TextDisabled("--");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(fd.path().extension().c_str());

                if (open) {
                    for (const auto& dir : std::filesystem::directory_iterator(fd)) {
                        fileTree(dir);
                    }
                    ImGui::TreePop();
                }
            } else {
                ImGui::TreeNodeEx(fd.path().filename().c_str(),
                                  treeNodeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet |
                                      ImGuiTreeNodeFlags_NoTreePushOnOpen);
                ImGui::TableNextColumn();
                ImGui::Text("%ju", std::filesystem::file_size(fd));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(fd.path().extension().c_str());
            }
        };

        for (const auto& dir : std::filesystem::directory_iterator(".")) {
            fileTree(dir);
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

} // namespace R3::editor
