#pragma once
#include "api/Types.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Pipeline.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/Window.hpp"

namespace R3 {

const struct RenderSpecification {
    const Window* window;
};

class Renderer {
protected:
    Renderer() = default;
    Renderer(RenderSpecification spec);
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
    Pipeline m_pipeline;

    RenderSpecification m_spec;
};

} // namespace R3