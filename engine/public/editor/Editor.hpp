#pragma once

#include <R3>
#include "render/CommandBuffer.hpp"

namespace R3::editor {

struct R3_API EditorSpecification {
    const Window& window;
    const Instance& instance;
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const RenderPass& renderPass;
};

class R3_API Editor {
public:
    DEFAULT_CONSTRUCT(Editor);
    NO_COPY(Editor);
    DEFAULT_MOVE(Editor);

    Editor(const EditorSpecification& spec);

    ~Editor();

    void beginFrame();

    void endFrame();

    void drawFrame(const CommandBuffer& commandBuffer);

    void setContentScale(float scale);

    void displayDeltaTime(double dt);

    void initializeDocking();

    void displayHierarchy();

    void displayProperties();

    void displaySceneManager();

    void setCurrentEntity(uuid32 id) { m_currentEntity = id; }

    [[nodiscard]] constexpr uuid32 currentEntity() const { return m_currentEntity; }

private:
    Ref<void> m_descriptorPool; // void Handle because it's non-owning
    Ref<const LogicalDevice> m_logicalDevice;
    uuid32 m_currentEntity = ~uuid32(0);
};

} // namespace R3::editor
