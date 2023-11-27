#pragma once
#include "api/Types.hpp"
#include "render/CommandPool.hpp"
#include "render/Fence.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/PipelineLayout.hpp"
#include "render/RenderPass.hpp"
#include "render/Semaphore.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/Window.hpp"

namespace R3 {

struct RendererSpecification {
    const Window* window;
};

class Renderer {
public:
    Renderer() = default;

    void create(RendererSpecification spec);
    void destroy();

    void render();

private:
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    GraphicsPipeline m_graphicsPipeline;
    PipelineLayout m_pipelineLayout;
    std::vector<Framebuffer> m_framebuffers;
    CommandPool m_commandPool;

    Semaphore m_imageAvailable;
    Semaphore m_renderFinished;
    Fence m_inFlight;

    RendererSpecification m_spec;
};

} // namespace R3