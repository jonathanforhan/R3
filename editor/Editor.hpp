#pragma once

#include <span>
#include <vulkan/vulkan.h>
#include <engine/api/Class.hpp>
#include <engine/core/Entity.hpp>
#include <engine/editor/Editor.hpp>
#include <engine/render/CommandBuffer.hpp>
#include <engine/render/RenderContext.hpp>
#include <engine/render/Window.hpp>

namespace R3 {

class R3_API Editor : public IEditor {
public:
    R3_COPY_DELETE(Editor);
    R3_MOVE_DELETE(Editor);

    Editor(Window& window, IRenderContext& ctx);

    virtual ~Editor() noexcept override;

    virtual void recordFrame(double dt) override;

    virtual void draw(ICommandBuffer& cmd) override;

    virtual bool uiFocused() const override;

    std::span<const uint32> selectedEntityIDs() const override { return m_selectedEntityIDs; }

    void beginFrame();

    void endFrame();

    void setContentScale(float scale);

    void displayDeltaTime(double dt);

    void initializeDocking();

    void displayHierarchy();

    void displayProperties();

    void displaySceneManager();

private:
    void setupEventListeners();

    void hierarchyHelper(Entity entity);

    void testImGuizmo();

private:
    IRenderContext& m_ctx;
    VkDescriptorPool m_descriptorPool       = VK_NULL_HANDLE;
    bool m_uiFocused                        = false;
    bool m_isMouseClickedQueued             = false;
    int32 m_queuedMouseClickX               = 0;
    int32 m_queuedMouseClickY               = 0;
    uint32 m_hoveredEntityID                = 0xFFFF'FFFF;
    std::vector<uint32> m_selectedEntityIDs = {};
    int m_guizmoOperation;
};

} // namespace R3
