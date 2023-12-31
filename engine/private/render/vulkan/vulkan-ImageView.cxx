#if R3_VULKAN

#include "render/ImageView.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/Image.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

ImageView::ImageView(const ImageViewSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    CHECK(spec.mipLevels != 0);

    const vk::ImageViewCreateInfo imageViewCreateInfo = {
        .sType = vk::StructureType::eImageViewCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .image = spec.image.as<vk::Image>(),
        .viewType = vk::ImageViewType::e2D,
        .format = vk::Format(spec.format),
        .components =
            {
                .r = vk::ComponentSwizzle::eIdentity,
                .g = vk::ComponentSwizzle::eIdentity,
                .b = vk::ComponentSwizzle::eIdentity,
                .a = vk::ComponentSwizzle::eIdentity,
            },
        .subresourceRange =
            {
                .aspectMask = (vk::ImageAspectFlags)spec.aspectMask,
                .baseMipLevel = 0,
                .levelCount = spec.mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    setHandle(m_logicalDevice->as<vk::Device>().createImageView(imageViewCreateInfo));
}

ImageView::~ImageView() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyImageView(as<vk::ImageView>());
    }
}

} // namespace R3

#endif // R3_VULKAN