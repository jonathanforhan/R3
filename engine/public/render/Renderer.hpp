#pragma once

#include "api/Types.hpp"
#include "render/CommandPool.hpp"
#include "render/DepthBuffer.hpp"
#include "render/DescriptorPool.hpp"
#include "render/Fence.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/RenderPass.hpp"
#include "render/RenderSpecification.hpp"
#include "render/Semaphore.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/UniformBuffer.hpp"
#include "render/Window.hpp"
#include "components/ModelComponent.hpp"

namespace R3 {

struct RendererSpecification {
    Window& window;
};

class Renderer {
public:
    Renderer(RendererSpecification spec);

    [[nodiscard]] Ref<const PhysicalDevice> physicalDevice() const { return &m_physicalDevice; }
    [[nodiscard]] Ref<const LogicalDevice> logicalDevice() const { return &m_logicalDevice; }
    [[nodiscard]] Ref<const Swapchain> swapchain() const { return &m_swapchain; }
    [[nodiscard]] Ref<const CommandPool> commandPool() const { return &m_commandPool; }

    void render(double dt);
    void waitIdle() const;

private:
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    DescriptorPool m_descriptorPool;
    GraphicsPipeline m_graphicsPipeline;
    std::vector<Framebuffer> m_framebuffers;
    CommandPool m_commandPool;          // used for the render command buffers
    CommandPool m_commandPoolTransient; // used for small command buffer operations like CPU -> GPU copy

    Semaphore m_imageAvailable[MAX_FRAMES_IN_FLIGHT];
    Semaphore m_renderFinished[MAX_FRAMES_IN_FLIGHT];
    Fence m_inFlight[MAX_FRAMES_IN_FLIGHT];
    uint32 m_currentFrame = 0;

    // TODO
    std::vector<UniformBuffer> m_uniformBuffers;
    DepthBuffer m_depthBuffer;

    RendererSpecification m_spec;

public:
    ModelComponent _Model;
};

} // namespace R3