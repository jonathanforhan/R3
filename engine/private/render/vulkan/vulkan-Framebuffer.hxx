////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Framebuffer.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Result.hpp>
#include <span>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Vulkan Framebuffer Specification
struct FramebufferSpecification {
    const LogicalDevice& device;        ///< Valid LogicalDevice.
    const RenderPass& renderPass;       ///< Valid RenderPass.
    std::span<VkImageView> attachments; ///< Render attachment views.
    VkExtent2D extent;                  ///< Swapchain extent.
};

/// @brief Vulkan Framebuffer RAII wrapper.
/// Framebuffers represent memory attachments used by RenderPass.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFramebuffer.html
class Framebuffer : public VulkanObject<VkFramebuffer> {
public:
    DEFAULT_CONSTRUCT(Framebuffer);
    NO_COPY(Framebuffer);
    DEFAULT_MOVE(Framebuffer);

    /// @brief Create Framebuffer from spec with given attachments.
    /// @param spec
    /// @return Framebuffer | InitializationFailure
    static Result<Framebuffer> create(const FramebufferSpecification& spec);

    /// @brief Destroy Framebuffer
    ~Framebuffer();

private:
    VkDevice m_device   = VK_NULL_HANDLE;
    VkExtent2D m_extent = {};
};

} // namespace R3::vulkan

#endif // R3_VULKAN
