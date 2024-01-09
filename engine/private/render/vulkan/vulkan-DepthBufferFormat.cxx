#if R3_VULKAN

#include "vulkan-DepthBufferFormat.hxx"
#include "api/Ensure.hpp"

namespace R3::vulkan {

vk::Format getSupportedDepthFormat(vk::PhysicalDevice gpu, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    static constexpr vk::Format formats[] = {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
    };

    auto it = std::ranges::find_if(formats, [=](const auto& format) {
        const vk::FormatProperties formatProperties = gpu.getFormatProperties(format);

        switch (tiling) {
            case vk::ImageTiling::eLinear:
                return ((formatProperties.linearTilingFeatures & features) == features);
            case vk::ImageTiling::eOptimal:
                return ((formatProperties.optimalTilingFeatures & features) == features);
            default:
                return false;
        }
    });

    ENSURE(it != std::end(formats));
    return *it;
}

} // namespace R3::vulkan

#endif // R3_VULKAN