#if R3_VULKAN

#include "render/Image.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

std::vector<Image> Image::acquireImages(const ImageSpecification& spec) {
    std::vector<vk::Image> swapchainImages =
        spec.logicalDevice->as<vk::Device>().getSwapchainImagesKHR(spec.swapchain->as<vk::SwapchainKHR>());

    std::vector<Image> images(swapchainImages.size());

    for (uint32 i = 0; i < swapchainImages.size(); i++) {
        images[i].setHandle(swapchainImages[i]);
    }

    return images;
}

} // namespace R3

#endif // R3_VULKAN