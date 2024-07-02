#if R3_VULKAN

#include "vulkan-Buffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <expected>
#include <tuple>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<void> Buffer::initialize(const BufferSpecification& spec) {
    return Result<void>();
}

Buffer::~Buffer() {
    vkDestroyBuffer(m_device, VulkanObject<VkBuffer>::vk(), nullptr);
    vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::vk(), nullptr);
}

Result<std::tuple<VkBuffer, VkDeviceMemory>> Buffer::allocate() {
    return Result<std::tuple<VkBuffer, VkDeviceMemory>>();
}

constexpr bool Buffer::valid() const {
    return VulkanObject<VkBuffer>::valid() && VulkanObject<VkDeviceMemory>::valid();
}

} // namespace R3::vulkan

#endif // R3_VULKAN
