#if R3_VULKAN

#include "render/Image.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

std::vector<Image> Image::acquireImages(const ImageSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);

    uint32 swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(spec.logicalDevice->handle<VkDevice>(),
                            spec.swapchain->handle<VkSwapchainKHR>(),
                            &swapchainImageCount,
                            nullptr);
    CHECK(swapchainImageCount != 0);

    std::vector<VkImage> swapchainImages(swapchainImageCount);
    vkGetSwapchainImagesKHR(spec.logicalDevice->handle<VkDevice>(),
                            spec.swapchain->handle<VkSwapchainKHR>(),
                            &swapchainImageCount,
                            swapchainImages.data());

    std::vector<Image> images(swapchainImageCount);

    for (uint32 i = 0; i < swapchainImages.size(); i++) {
        images[i].setHandle(swapchainImages[i]);
    }

    return images;
}

} // namespace R3

#endif // R3_VULKAN