#if R3_VULKAN

#include "vulkan-DepthBufferFormat.hxx"
#include "api/Ensure.hpp"

namespace R3::vulkan {

vk::Format getSupportedDepthFormat(vk::PhysicalDevice gpu, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    constexpr vk::Format formats[] = {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
    };

    for (vk::Format format : formats) {
        const vk::FormatProperties formatProperties = gpu.getFormatProperties(format);

        switch (tiling) {
            case vk::ImageTiling::eLinear:
                if ((formatProperties.linearTilingFeatures & features) == features)
                    return format;
                break;
            case vk::ImageTiling::eOptimal:
                if ((formatProperties.optimalTilingFeatures & features) == features)
                    return format;
                break;
            default:
                continue;
        }
    }

    ENSURE(false);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
