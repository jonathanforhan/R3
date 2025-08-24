#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"

namespace R3::vulkan {

class Framebuffer {
public:
    R3_CTOR_DEFAULT(Framebuffer);
    R3_COPY_DELETE(Framebuffer);
    R3_MOVE_DEFAULT(Framebuffer);

    Framebuffer(RenderContext& ctx,
                RenderPass& renderPass,
                std::span<const VkImageView> attachments,
                VkExtent2D extent);

    ~Framebuffer() noexcept;

    VkFramebuffer framebuffer() const noexcept { return m_framebuffer; }

private:
    Handle<VkDevice> m_device;
    Handle<VkFramebuffer> m_framebuffer;
};

} // namespace R3::vulkan