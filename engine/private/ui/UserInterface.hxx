#pragma once

#include <R3>
#include <vector>
#include "render/RenderApi.hxx"
#include "render/ShaderObjects.hxx"

namespace R3 {

struct UserInterfaceSpecification {
    const Window& window;
    const Instance& instance;
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const RenderPass& renderPass;
};

class UserInterface {
public:
    DEFAULT_CONSTRUCT(UserInterface);
    NO_COPY(UserInterface);
    DEFAULT_MOVE(UserInterface);

    UserInterface(const UserInterfaceSpecification& spec);

    ~UserInterface();

    static void beginFrame();

    static void endFrame();

    void drawFrame(const CommandBuffer& commandBuffer);

    static void displayDeltaTime(double dt);

    static void lightEditor();

    static void transformEditor();

private:
    Ref<void> m_descriptorPool; // void Handle because it's non-owning
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3