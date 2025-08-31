#pragma once

#include <vulkan/vulkan.h>
#include <engine/api/Class.hpp>
#include <engine/core/Entity.hpp>
#include <engine/editor/Editor.hpp>
#include <engine/render/RenderContext.hpp>
#include <engine/render/Window.hpp>
#include <engine/render/vulkan/vulkan-CommandBuffer.hpp>

namespace R3 {

class R3_API Editor : public IEditor {
public:
    R3_COPY_DELETE(Editor);
    R3_MOVE_DELETE(Editor);

    Editor(Window& window, IRenderContext& ctx);

    virtual ~Editor() noexcept override;

    virtual void recordFrame(double dt) override;

    virtual void draw(vulkan::CommandBuffer& cmd) override;

    virtual bool uiFocused() const override;

    void beginFrame();

    void endFrame();

    void setContentScale(float scale);

    void displayDeltaTime(double dt);

    void initializeDocking();

    void displayHierarchy();

    void displayProperties();

    void displaySceneManager();

private:
    void hierarchyHelper(Entity entity);

    void testImGuizmo();

private:
    IRenderContext& m_ctx;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    bool m_uiFocused{false};
};

} // namespace R3
