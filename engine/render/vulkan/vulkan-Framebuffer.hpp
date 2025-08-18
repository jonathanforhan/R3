#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"

namespace R3::vulkan {

class Framebuffer {
public:
    void create(RenderContext& ctx,
                RenderPass& renderPass,
                std::span<const VkImageView> attachments,
                VkExtent2D extent);

    void destroy() noexcept;

    VkFramebuffer framebuffer() const noexcept { return m_framebuffer; }

private:
    VkDevice m_device;
    VkFramebuffer m_framebuffer;
};

} // namespace R3::vulkan