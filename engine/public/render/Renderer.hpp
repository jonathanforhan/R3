#pragma once

#include "api/Types.hpp"
#include "render/CommandPool.hpp"
#include "render/DescriptorPool.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/Fence.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/PipelineLayout.hpp"
#include "render/RenderPass.hpp"
#include "render/RenderSpecification.hpp"
#include "render/Semaphore.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/TextureBuffer.hpp"
#include "render/UniformBuffer.hpp"
#include "render/VertexBuffer.hpp"
#include "render/Window.hpp"

namespace R3 {

struct RendererSpecification {
    Window& window;
};

class Renderer {
public:
    Renderer(RendererSpecification spec);

    void render();
    void waitIdle() const;

private:
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    DescriptorSetLayout m_descriptorSetLayout;
    DescriptorPool m_descriptorPool;
    PipelineLayout m_pipelineLayout;
    GraphicsPipeline m_graphicsPipeline;
    std::vector<Framebuffer> m_framebuffers;
    CommandPool m_commandPool;          // used for the render command buffers
    CommandPool m_commandPoolTransient; // used for small command buffer operations like CPU -> GPU copy

    Semaphore m_imageAvailable[MAX_FRAMES_IN_FLIGHT];
    Semaphore m_renderFinished[MAX_FRAMES_IN_FLIGHT];
    Fence m_inFlight[MAX_FRAMES_IN_FLIGHT];
    uint32 m_currentFrame = 0;

    // TODO
    std::vector<VertexBuffer> m_vertexBuffers;
    std::vector<IndexBuffer<uint32>> m_indexBuffers;
    std::vector<UniformBuffer> m_uniformBuffers;
    TextureBuffer m_textureBuffer;

    RendererSpecification m_spec;
};

} // namespace R3