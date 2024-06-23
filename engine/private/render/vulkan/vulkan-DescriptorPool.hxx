#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct DescriptorPoolSpecification {
    const LogicalDevice& device;
    uint32 descriptorSetCount;
    std::span<VkDescriptorSetLayoutBinding> layoutBindings;
};

class DescriptorPool : public VulkanObject<VkDescriptorPool>, public VulkanObject<VkDescriptorSetLayout> {
public:
    using VulkanObject<VkDescriptorPool>::vk;

public:
    DEFAULT_CONSTRUCT(DescriptorPool);
    NO_COPY(DescriptorPool);
    DEFAULT_MOVE(DescriptorPool);

    DescriptorPool(const DescriptorPoolSpecification& spec);

    ~DescriptorPool();

    VkDescriptorSetLayout layout() const { return VulkanObject<VkDescriptorSetLayout>::m_handle; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
