#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct RenderPassSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
};

class RenderPass : public NativeRenderObject {
public:
    void create(const RenderPassSpecification& spec);
    void destroy();

private:
    RenderPassSpecification m_spec;
};

} // namespace R3