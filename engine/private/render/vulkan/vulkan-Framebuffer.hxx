#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <span>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct FramebufferSpecification {
    const LogicalDevice& device;
    const RenderPass& renderPass;
    std::span<VkImageView> attachments;
    VkExtent2D extent;
};

class Framebuffer : public VulkanObject<VkFramebuffer> {
public:
    DEFAULT_CONSTRUCT(Framebuffer);
    NO_COPY(Framebuffer);
    DEFAULT_MOVE(Framebuffer);

    Framebuffer(const FramebufferSpecification& spec);

    ~Framebuffer();

    constexpr VkExtent2D extent() const { return m_extent; }

private:
    VkDevice m_device   = VK_NULL_HANDLE;
    VkExtent2D m_extent = {};
};

} // namespace R3::vulkan

#endif // R3_VULKAN
