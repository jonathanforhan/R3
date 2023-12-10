#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct RenderPassSpecification {
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
    RenderPassSpecification m_spec;
};

} // namespace R3