#if R3_VULKAN

#include "render/DepthBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "render/Image.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Swapchain.hpp"
#include "vulkan-DepthBufferFormat.hxx"

namespace R3 {

DepthBuffer::DepthBuffer(const DepthBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    const vk::Format depthFormat = vulkan::getSupportedDepthFormat(spec.physicalDevice.as<vk::PhysicalDevice>(),
                                                                   vk::ImageTiling::eOptimal,
                                                                   vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    const uvec2 extent = spec.swapchain.extent();

    const ImageAllocateSpecification imageAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = extent.x * extent.y * sizeof(float),
        .format = Format(depthFormat),
        .width = extent.x,
        .height = extent.y,
        .mipLevels = 1,
        .imageFlags = ImageUsage::DepthStencilAttachment,
        .memoryFlags = MemoryProperty::DeviceLocal,
    };
    auto&& [image, memory] = Image::allocate(imageAllocateSpecification);

    setHandle(image.handle());
    setDeviceMemory(memory.handle());

    Image img(handle());
    m_imageView = ImageView({
        .logicalDevice = *m_logicalDevice,
        .image = img,
        .format = Format(depthFormat),
        .mipLevels = 1,
        .aspectMask = ImageAspect::Depth,
    });
}

DepthBuffer::~DepthBuffer() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyImage(as<vk::Image>());
        m_logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

} // namespace R3

#endif // R3_VULKAN