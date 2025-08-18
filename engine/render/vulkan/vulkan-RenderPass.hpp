#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class RenderPass {
public:
    void create(RenderContext& ctx, std::span<const VkAttachmentDescription> attachments);

    void destroy() noexcept;

    VkRenderPass renderPass() const noexcept { return m_renderPass; }

private:
    VkDevice m_device         = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

} // namespace R3::vulkan