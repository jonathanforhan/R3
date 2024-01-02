#pragma once

#include <R3>
#include <vector>
#include "render/RenderApi.hxx"
#include "render/ShaderObjects.hxx"

namespace R3::editor {

struct EditorSpecification {
    const Window& window;
    const Instance& instance;
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const RenderPass& renderPass;
};

class Editor {
public:
    DEFAULT_CONSTRUCT(Editor);
    NO_COPY(Editor);
    DEFAULT_MOVE(Editor);

    Editor(const EditorSpecification& spec);

    ~Editor();

    void beginFrame();

    void endFrame();

    void drawFrame(const CommandBuffer& commandBuffer);

    void displayDeltaTime(double dt);

    void initializeDocking();

    void displayHierarchy();

    void displayProperties();

private:
    Ref<void> m_descriptorPool; // void Handle because it's non-owning
    Ref<const LogicalDevice> m_logicalDevice;
    int64 m_currentEntity = -1;
};

} // namespace R3::editor