#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct RenderPassSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
};

class RenderPass : public NativeRenderObject {
public:
    RenderPass() = default;
    RenderPass(const RenderPassSpecification& spec);
    RenderPass(RenderPass&&) noexcept = default;
    RenderPass& operator=(RenderPass&&) noexcept = default;
    ~RenderPass();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3