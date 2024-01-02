#if R3_VULKAN

#include "render/DepthBuffer.hxx"

#include <vulkan/vulkan.hpp>
#include "render/Image.hxx"
#include "render/LogicalDevice.hxx"
#include "render/PhysicalDevice.hxx"
#include "render/Swapchain.hxx"
#include "vulkan-DepthBufferFormat.hxx"

namespace R3 {

DepthBuffer::DepthBuffer(const DepthBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_extent(spec.extent),
      m_sampleCount(spec.sampleCount) {
    m_format = Format(vulkan::getSupportedDepthFormat(spec.physicalDevice.as<vk::PhysicalDevice>(),
                                                      vk::ImageTiling::eOptimal,
                                                      vk::FormatFeatureFlagBits::eDepthStencilAttachment));

    const ImageAllocateSpecification imageAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = m_extent.x * m_extent.y * sizeof(float),
        .format = m_format,
        .width = m_extent.x,
        .height = m_extent.y,
        .mipLevels = 1,
        .samples = m_sampleCount,
        .imageFlags = ImageUsage::DepthStencilAttachment,
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