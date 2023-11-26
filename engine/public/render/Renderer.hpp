#pragma once
#include "api/Types.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/PipelineLayout.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/Window.hpp"

namespace R3 {

struct RendererSpecification {
    const Window* window;
};

class Renderer {
protected:
    Renderer() = default;
    Renderer(RendererSpecification spec);
    friend class Engine;

public:
    Renderer(const Renderer&) = delete;
    void operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

    ~Renderer();

private:
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    Swapchain m_swapchain;
    GraphicsPipeline m_graphicsPipeline;
    PipelineLayout m_pipelineLayout;

    RendererSpecification m_spec;
};

} // namespace R3