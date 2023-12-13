#if R3_VULKAN

#include "render/ImageView.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/Image.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

ImageView::ImageView(const ImageViewSpecification& spec)
    : m_spec(spec) {

    const VkImageViewCreateInfo imageViewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .image = m_spec.image->as<VkImage>(),
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

    VkImageView tmp;
    VkResult result = vkCreateImageView(m_spec.logicalDevice->as<VkDevice>(), &imageViewCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

ImageView::~ImageView() {
    if (validHandle()) {
        vkDestroyImageView(m_spec.logicalDevice->as<VkDevice>(), as<VkImageView>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN