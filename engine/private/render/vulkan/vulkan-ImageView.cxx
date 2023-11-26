#if R3_VULKAN

#include "render/ImageView.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "render/Image.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

void ImageView::create(const ImageViewSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    CHECK(spec.image != nullptr);
    m_spec = spec;

    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .image = m_spec.image->handle<VkImage>(),
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = (VkFormat)m_spec.swapchain->surfaceFormat(),
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };
}

void ImageView::destroy() {
    vkDestroyImageView(m_spec.logicalDevice->handle<VkDevice>(), handle<VkImageView>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN