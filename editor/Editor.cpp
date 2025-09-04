#include "Editor.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <ImGuizmo.h>

#include <filesystem>
#include <format>
#include <iterator>
#include <string>
#include <vulkan/vulkan.h>
#include <engine/api/Types.hpp>
#include <engine/components/HierarchyComponent.hpp>
#include <engine/components/MetadataComponent.hpp>
#include <engine/core/Engine.hpp>
#include <engine/core/Entity.hpp>
#include <engine/core/EventHandler.hpp>
#include <engine/core/World.hpp>
#include <engine/render/RenderContext.hpp>
#include <engine/render/Window.hpp>
#include <engine/render/WindowEvents.hpp>
#include <engine/render/vulkan/vulkan-CommandBuffer.hpp>
#include <engine/render/vulkan/vulkan-RenderContext.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace R3 {

static constexpr auto GUI_BOARDERLESS =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

Editor::Editor(Window& window, IRenderContext& ctx_)
    : m_ctx{ctx_} {
    vulkan::RenderContext& ctx = static_cast<vulkan::RenderContext&>(m_ctx);

    const VkDescriptorPoolSize poolSizes[] = {
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
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets       = 1000,
        .poolSizeCount = (uint32)std::size(poolSizes),
        .pPoolSizes    = poolSizes,
    };

    vkCreateDescriptorPool(ctx.device(), &descriptorPoolInfo, nullptr, &m_descriptorPool);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.glfw(), true);

    VkFormat swapchainFormat = ctx.swapchainFormat();

    ImGui_ImplVulkan_InitInfo initInfo = {
        .Instance            = ctx.instance(),
        .PhysicalDevice      = ctx.physicalDevice(),
        .Device              = ctx.device(),
        .QueueFamily         = ctx.graphicsQueueIndex(),
        .Queue               = ctx.graphicsQueue(),
        .DescriptorPool      = m_descriptorPool,
        .RenderPass          = VK_NULL_HANDLE,
        .MinImageCount       = ctx.maxFramesInFlight(),
        .ImageCount          = ctx.maxFramesInFlight(),
        .MSAASamples         = ctx.queryMaxUsableSampleCount(),
        .Subpass             = 0,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo =
            {
                .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                .pNext                   = nullptr,
                .viewMask                = 0,
                .colorAttachmentCount    = 1,
                .pColorAttachmentFormats = &swapchainFormat,
                .depthAttachmentFormat   = ctx.queryDepthFormat(),
                .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
            },
    };

    ImGui_ImplVulkan_Init(&initInfo);

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    // setup fonts
    io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Medium.ttf", 16.5f * 1.5f);
    io.Fonts->Build();

    GEventHandler()->bindEventListener("window-content-scale", [this](const Event<WindowResizeEvent>& e) noexcept {
        setContentScale((e.data.width + e.data.height) / 2.0f);
    });
}

Editor::~Editor() {
    vulkan::RenderContext& ctx = static_cast<vulkan::RenderContext&>(m_ctx);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_descriptorPool) {
        vkDestroyDescriptorPool(ctx.device(), m_descriptorPool, nullptr);
    }
}

void Editor::recordFrame(double dt) {
    beginFrame();

    m_uiFocused = false;
    if (ImGui::GetCurrentContext()) {
        if (ImGui::GetIO().WantCaptureKeyboard) {
            m_uiFocused = true;
        }
    }

    // ImGui::ShowDemoWindow();
    initializeDocking();

    // testImGuizmo();

    displayHierarchy();
    // displayProperties();
    // displaySceneManager();
    displayDeltaTime(dt);
    endFrame();
}

void Editor::draw(vulkan::CommandBuffer& cmd) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.commandBuffer());
}

bool Editor::uiFocused() const {
    return m_uiFocused;
}

void Editor::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    auto& io          = ImGui::GetIO();
    float scaleFactor = io.DisplayFramebufferScale.x;
    ImGui::GetStyle().ScaleAllSizes(scaleFactor);
}

void Editor::endFrame() {
    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void Editor::setContentScale(float scale) {
    ImGui::GetIO().FontGlobalScale = scale;
}

void Editor::displayDeltaTime(double dt) {
    ImGui::Begin("Delta Time", nullptr, GUI_BOARDERLESS);
    ImGui::SetWindowPos(ImVec2(10, 10));
    ImGui::Text("%.02f ms\n%i FPS", dt * 1000.0f, (int)(1.0f / dt));
    ImGui::End();
}

void Editor::initializeDocking() {
    ImGuiDockNodeFlags dockspaceFlags = 0;
    dockspaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
    dockspaceFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground; // This is key!

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(0.0f, 0.0f), dockspaceFlags);
    ImGui::End();
}

void Editor::displayHierarchy() {
    // Hierarchy Panel
    if (ImGui::Begin("Hierarchy")) {
        GWorld()->registry().view<HierarchyComponent>().each([this](Entity entity, const HierarchyComponent& hier) {
            if (hier.parent == entt::null) {
                hierarchyHelper(entity);
            }
        });
    }
    ImGui::End();
}

void Editor::displayProperties() {
#if 0
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
            const bool scaleLock    = ImGui::GetIO().KeyCtrl;
            const auto sliderFlags  = scaleLock ? ImGuiSliderFlags_NoInput : ImGuiSliderFlags_None;

            // Edit position
            vec3 position = editorComponent.position;
            ImGui::DragFloat3("position", glm::value_ptr(position), sensitivity, 0, 0, "%.4f", sliderFlags);
            vec3 deltaPosition       = position - editorComponent.position;
            editorComponent.position = position;
            ImGui::Spacing();

            // Edit Rotation
            vec3 rotation = editorComponent.rotation;
            ImGui::DragFloat3("rotation", glm::value_ptr(rotation), sensitivity, 0, 0, "%.4f", sliderFlags);
            vec3 deltaRotation       = rotation - editorComponent.rotation;
            rotation.x               = rotation.x < 0 ? 360 : rotation.x >= 360 ? 0 : rotation.x;
            rotation.y               = rotation.y < 0 ? 360 : rotation.y >= 360 ? 0 : rotation.y;
            rotation.z               = rotation.z < 0 ? 360 : rotation.z >= 360 ? 0 : rotation.z;
            editorComponent.rotation = rotation;
            mat4 rotationMatrix      = mat4(1.0f);
            rotationMatrix           = glm::rotate(rotationMatrix, glm::radians(deltaRotation.x), vec3(1, 0, 0));
            rotationMatrix           = glm::rotate(rotationMatrix, glm::radians(deltaRotation.y), vec3(0, 1, 0));
            rotationMatrix           = glm::rotate(rotationMatrix, glm::radians(deltaRotation.z), vec3(0, 0, 1));
            ImGui::Spacing();

            // Edit scale
            auto scale = editorComponent.scale;
            ImGui::DragFloat3("scale", glm::value_ptr(scale), sensitivity, 0, 0, "%.4f", sliderFlags);
            if (scaleLock) {
                vec3 dscale = scale - editorComponent.scale;
                // two of the components will be zero so we can sum them, no need to check
                float delta = dscale.x + dscale.y + dscale.z;
                scale       = editorComponent.scale + vec3(delta);
            }
            scale.x               = scale.x <= 0 ? 0.0001f : scale.x;
            scale.y               = scale.y <= 0 ? 0.0001f : scale.y;
            scale.z               = scale.z <= 0 ? 0.0001f : scale.z;
            vec3 deltaScale       = scale / editorComponent.scale;
            editorComponent.scale = scale;
            ImGui::Spacing();

            mat4& transform = entityView.get<TransformComponent>();
            transform       = transform * rotationMatrix;
            transform[3] += vec4(deltaPosition, 0.0f);
            transform = glm::scale(transform, deltaScale);
        }
    }
    ImGui::End();
#endif
}

void Editor::displaySceneManager() {
    const float TEXT_BASE_WIDTH            = ImGui::CalcTextSize("A").x;
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
                const bool open = ImGui::TreeNodeEx(fd.path().filename().string().c_str(), treeNodeFlags);
                ImGui::TableNextColumn();
                ImGui::TextDisabled("--");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(fd.path().extension().string().c_str());

                if (open) {
                    for (const auto& dir : std::filesystem::directory_iterator(fd)) {
                        fileTree(dir);
                    }
                    ImGui::TreePop();
                }
            } else {
                ImGui::TreeNodeEx(fd.path().filename().string().c_str(),
                                  treeNodeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet |
                                      ImGuiTreeNodeFlags_NoTreePushOnOpen);
                ImGui::TableNextColumn();
                ImGui::Text("%ju", std::filesystem::file_size(fd));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(fd.path().extension().string().c_str());
            }
        };

        for (const auto& dir : std::filesystem::directory_iterator(".")) {
            fileTree(dir);
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

void Editor::hierarchyHelper(Entity entity) {
    std::string name;

    if (const MetadataComponent* metadata = GWorld()->registry().try_get<MetadataComponent>(entity)) {
        name = metadata->name;
    }

    if (name.empty()) {
        name = std::format("{}", (uint32)entity);
    }

    ImGui::PushID((int)entity);

    bool parent = GWorld()->registry().try_get<HierarchyComponent>(entity) != nullptr;

    if (ImGui::TreeNodeEx(name.c_str(), parent ? 0 : ImGuiTreeNodeFlags_Leaf)) {
        if (const HierarchyComponent* h = GWorld()->registry().try_get<HierarchyComponent>(entity)) {
            for (Entity child : h->children) {
                hierarchyHelper(child);
            }
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void Editor::testImGuizmo() {
    static glm::mat4 testMatrix                      = glm::mat4(1.0f); // Identity matrix
    static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
    static ImGuizmo::MODE currentGizmoMode           = ImGuizmo::WORLD;

    // Create a test window
    if (ImGui::Begin("ImGuizmo Test")) {
        // Gizmo operation buttons
        if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
            currentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
            currentGizmoOperation = ImGuizmo::SCALE;

        // Mode toggle
        if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
            currentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
            currentGizmoMode = ImGuizmo::WORLD;

        ImGui::Separator();

        // Display current matrix values
        ImGui::Text("Matrix values:");
        for (int i = 0; i < 4; i++) {
            ImGui::Text("Row %d: %.2f, %.2f, %.2f, %.2f",
                        i,
                        testMatrix[i][0],
                        testMatrix[i][1],
                        testMatrix[i][2],
                        testMatrix[i][3]);
        }

        // Reset button
        if (ImGui::Button("Reset Matrix")) {
            testMatrix = glm::mat4(1.0f);
        }

        ImGui::Separator();

        // Set up ImGuizmo for this frame
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        // Create simple view and projection matrices for testing
        glm::mat4 view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), // Camera position
                                     glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
                                     glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
        );

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),                 // FOV
                                                io.DisplaySize.x / io.DisplaySize.y, // Aspect ratio
                                                0.1f,                                // Near plane
                                                100.0f                               // Far plane
        );

        // Draw the gizmo
        ImGuizmo::Manipulate(glm::value_ptr(view),
                             glm::value_ptr(projection),
                             currentGizmoOperation,
                             currentGizmoMode,
                             glm::value_ptr(testMatrix));

        // Show if gizmo is being used
        if (ImGuizmo::IsUsing()) {
            ImGui::Text("Gizmo is being manipulated!");
        }

        // Show if gizmo is hovered
        if (ImGuizmo::IsOver()) {
            ImGui::Text("Mouse is over gizmo");
        }
    }
    ImGui::End();
}

} // namespace R3
