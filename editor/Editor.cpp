#include "Editor.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <ImGuizmo.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <iterator>
#include <ranges>
#include <string>
#include <vulkan/vulkan.h>
#include <engine/api/Types.hpp>
#include <engine/components/HierarchyComponent.hpp>
#include <engine/components/MetadataComponent.hpp>
#include <engine/components/TransformComponent.hpp>
#include <engine/core/Engine.hpp>
#include <engine/core/Entity.hpp>
#include <engine/core/EventHandler.hpp>
#include <engine/core/World.hpp>
#include <engine/input/InputEvents.hpp>
#include <engine/render/CommandBuffer.hpp>
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
        .MSAASamples         = (VkSampleCountFlagBits)ctx.queryMaxUsableSampleCount(),
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

    setupEventListeners();
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

    testImGuizmo();

    displayHierarchy();
    // displayProperties();
    // displaySceneManager();
    displayDeltaTime(dt);
    endFrame();
}

void Editor::draw(ICommandBuffer& cmd) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), ((vulkan::CommandBuffer&)cmd).commandBuffer());
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

void Editor::setupEventListeners() {
    // Listen for window content scale changes to adjust ImGui scaling
    GEventHandler()->bindEventListener(event::WindowContentScale, [this](const WindowResizeEvent& e) noexcept {
        setContentScale((e.width + e.height) / 2.0f);
    });

    // Queue mouse clicks to be processed on release, this allows for drag operations without affecting selection
    GEventHandler()->bindEventListener(event::MousePress, [this](const MouseButtonEvent& event) noexcept {
        m_queuedMouseClickX = static_cast<int32>(event.xpos);
        m_queuedMouseClickY = static_cast<int32>(event.ypos);
    });

    // Process queued mouse click on release, if the mouse hasn't moved much since the press then we consider it a click
    // and update selection
    GEventHandler()->bindEventListener(event::MouseRelease, [this](const MouseButtonEvent& event) noexcept {
        const int32 xpos = static_cast<int32>(event.xpos);
        const int32 ypos = static_cast<int32>(event.ypos);

        if (std::abs(m_queuedMouseClickX - xpos) < 4 && std::abs(m_queuedMouseClickY - ypos) < 4) {
            if (m_hoveredEntityID == 0xFFFF'FFFF) {
                m_selectedEntityIDs.clear();
                m_guizmoOperation = -1;
            } else if (event.modifiers & InputModifierFlags::Shift) {
                auto it = std::ranges::find(m_selectedEntityIDs, m_hoveredEntityID);
                if (it == m_selectedEntityIDs.end()) {
                    m_selectedEntityIDs.emplace_back(m_hoveredEntityID);
                }
            } else if (event.modifiers & InputModifierFlags::Control) {
                auto it = std::ranges::find(m_selectedEntityIDs, m_hoveredEntityID);
                if (it == m_selectedEntityIDs.end()) {
                    m_selectedEntityIDs.emplace_back(m_hoveredEntityID);
                } else {
                    m_selectedEntityIDs.erase(it);
                }
            } else {
                m_selectedEntityIDs = std::vector<uint32>{m_hoveredEntityID};
            }
        }
    });

    // Listen for hovered entity changes to update internal hovered entity state for selection on click
    GEventHandler()->bindEventListener(event::HoveredEntity, [this](HoveredEntityEvent hovered) noexcept {
        m_hoveredEntityID = hovered.entityID; //
    });

    GEventHandler()->bindEventListener(event::KeyPress, [this](const KeyboardEvent& e) noexcept {
        switch (e.key) {
            case Key::A:
                if (e.modifiers & InputModifierFlags::Control) {
                    // Select all entities on Ctrl + A
                    m_selectedEntityIDs.clear();
                    GWorld()->registry().view<MetadataComponent>().each([this](const MetadataComponent& metadata) {
                        m_selectedEntityIDs.emplace_back((uint32)metadata.entity);
                    });
                }
                break;
            case Key::R:
                if (!m_selectedEntityIDs.empty()) {
                    m_guizmoOperation = ImGuizmo::OPERATION::ROTATE;
                }
                break;
            case Key::G:
                if (!m_selectedEntityIDs.empty()) {
                    m_guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
                }
                break;
            case Key::S:
                if (!m_selectedEntityIDs.empty()) {
                    m_guizmoOperation = ImGuizmo::OPERATION::SCALE;
                }
                break;
            case Key::Escape:
                m_selectedEntityIDs.clear();
                m_guizmoOperation = -1;
                break;
            default:
                break;
        }
    });
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
    if (m_selectedEntityIDs.empty() || m_guizmoOperation < 0) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    fmat4 view       = GWorld()->camera().view();
    fmat4 projection = GWorld()->camera().projection();

    dmat4& model = GWorld()->registry().get<TransformComponent>((entt::entity)m_selectedEntityIDs.back()).transform();

    fmat4 before = static_cast<fmat4>(model);
    fmat4 after  = before;

    if (ImGuizmo::Manipulate(glm::value_ptr(view),
                             glm::value_ptr(projection),
                             static_cast<ImGuizmo::OPERATION>(m_guizmoOperation),
                             ImGuizmo::MODE::WORLD,
                             glm::value_ptr(after))) {
        fmat4 delta = after * glm::inverse(before);
        for (auto& entityID : m_selectedEntityIDs) {
            TransformComponent& transform = GWorld()->registry().get<TransformComponent>((entt::entity)entityID);
            transform.transform()         = delta * (fmat4)transform.transform();
        }
    }
}

} // namespace R3
