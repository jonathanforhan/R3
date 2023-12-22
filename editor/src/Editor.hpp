#pragma once

#include "render/RenderFwd.hpp"

namespace R3::editor {

struct EditorSpecification {
    Ref<const Window> window;
    Ref<const Instance> instance;
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const RenderPass> renderPass;
    Ref<const Queue> graphicsQueue;
    uint32 minImageCount;
    uint32 imageCount;
};

class Editor {
public:
    Editor();
    Editor(const Editor&) = delete;
    Editor(Editor&&) = delete;
    ~Editor();

    void newFrame();
    void drawUI(Ref<const CommandBuffer> commandBuffer);

private:
    Ref<const LogicalDevice> m_logicalDevice;
    void* m_descriptorPool = nullptr;
};

} // namespace R3::editor
