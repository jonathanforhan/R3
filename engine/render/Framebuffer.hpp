#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;
class RenderPass;

class Framebuffer {
public:
    void create(RenderContext& ctx, RenderPass& renderPass, std::span<const VkImageView> attachments, uvec2 extent);

    void destroy() noexcept;

    VkFramebuffer handle() noexcept { return m_framebuffer; }

private:
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    VkDevice m_device           = VK_NULL_HANDLE;
};

} // namespace R3