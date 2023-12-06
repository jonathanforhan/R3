#if R3_VULKAN

#include "render/ImageView.hpp"

#include <vulkan/vulkan.hpp>
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

    vk::ImageViewCreateInfo imageViewCreateInfo = {
        .sType = vk::StructureType::eImageViewCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .image = m_spec.image->handle<VkImage>(),
        .viewType = vk::ImageViewType::e2D,
        .format = (vk::Format)m_spec.swapchain->surfaceFormat(),
        .components =
            {
                .r = vk::ComponentSwizzle::eIdentity,
                .g = vk::ComponentSwizzle::eIdentity,
                .b = vk::ComponentSwizzle::eIdentity,
                .a = vk::ComponentSwizzle::eIdentity,
            },
        .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createImageView(imageViewCreateInfo));
}

void ImageView::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroyImageView(as<vk::ImageView>());
}

} // namespace R3

#endif // R3_VULKAN