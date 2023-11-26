#if R3_VULKAN

#include "render/RenderPass.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

void RenderPass::create(const RenderPassSpecification& spec) {
    CHECK(spec.swapchain != nullptr);
    m_spec = spec;

    #if 0
    VkAttachmentDescription colorAttachment = {
        .flags = 0U,
        .format = (VkFormat)m_spec.swapchain->surfaceFormat(),
        .samples = ,
        .loadOp = ,
        .storeOp = ,
        .stencilLoadOp = ,
        .stencilStoreOp = ,
        .initialLayout = ,
        .finalLayout = ,
    };
    #endif
}

void RenderPass::destroy() {
}

} // namespace R3

#endif // R3_VULKAN