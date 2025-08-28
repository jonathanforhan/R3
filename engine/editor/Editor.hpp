#pragma once

#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "render/RenderContext.hpp"
#include "render/Window.hpp"

namespace R3 {

class Editor {
public:
    R3_COPY_DELETE(Editor);
    R3_MOVE_DELETE(Editor);

    Editor(Window& window, IRenderContext& ctx);

    ~Editor() noexcept;

    void recordInterfaceFrame(double dt);

    void beginFrame();

    void endFrame();

    void setContentScale(float scale);

    void displayDeltaTime(double dt);

    void initializeDocking();

    void displayHierarchy();

    void displayProperties();

    void displaySceneManager();

private:
    IRenderContext& m_ctx;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
};

} // namespace R3
