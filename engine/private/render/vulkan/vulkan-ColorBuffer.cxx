#if R3_VULKAN

#include "render/ColorBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "render/Image.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

ColorBuffer::ColorBuffer(const ColorBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    Format colorFormat = spec.swapchain.surfaceFormat();

    const uvec2 extent = spec.swapchain.extent();

    const ImageAllocateSpecification imageAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = extent.x * extent.y * sizeof(float),
        .format = Format(colorFormat),
        .width = extent.x,
        .height = extent.y,
        .mipLevels = 1,
        .samples = spec.physicalDevice.sampleCount(),
        .imageFlags = ImageUsage::TransientAttachment | ImageUsage::ColorAttachment,
        .memoryFlags = MemoryProperty::DeviceLocal,
    };
    auto&& [image, memory] = Image::allocate(imageAllocateSpecification);

    setHandle(image.handle());
    setDeviceMemory(memory.handle());

    m_imageView = ImageView({
        .logicalDevice = *m_logicalDevice,
        .image = Image(handle()),
        .format = Format(colorFormat),
        .mipLevels = 1,
        .aspectMask = ImageAspect::Color,
    });
}

ColorBuffer::~ColorBuffer() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyImage(as<vk::Image>());
        m_logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

} // namespace R3

#endif // R3_VULKAN