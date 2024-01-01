#if R3_VULKAN

#include "render/ColorBuffer.hxx"

#include <vulkan/vulkan.hpp>
#include "render/Image.hxx"
#include "render/LogicalDevice.hxx"
#include "render/PhysicalDevice.hxx"
#include "render/Swapchain.hxx"

namespace R3 {

ColorBuffer::ColorBuffer(const ColorBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_format(spec.format),
      m_extent(spec.extent),
      m_sampleCount(spec.sampleCount) {
    const ImageAllocateSpecification imageAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = uint32(m_extent.x * m_extent.y * sizeof(float)),
        .format = m_format,
        .width = m_extent.x,
        .height = m_extent.y,
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
        .format = m_format,
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