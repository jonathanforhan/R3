#if R3_VULKAN

#include "vulkan-DescriptorPool.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

DescriptorPool::DescriptorPool(const DescriptorPoolSpecification& spec)
    : m_device(spec.device.vk()) {
    // DescriptorSetLayout
    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = {},
        .bindingCount = static_cast<uint32>(spec.layoutBindings.size()),
        .pBindings    = spec.layoutBindings.data(),
    };

    VkDescriptorSetLayout descriptorSetLayout;
    VkResult result =
        vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
    ENSURE(result == VK_SUCCESS);
    VulkanObject<VkDescriptorSetLayout>::m_handle = descriptorSetLayout;

    // DescriptorPool
    std::vector<VkDescriptorPoolSize> poolSizes;

    // if poolsize already contains type, add to it's count, else make new entry
    for (const VkDescriptorSetLayoutBinding& binding : spec.layoutBindings) {
        auto it = std::find_if(poolSizes.begin(), poolSizes.end(), [&binding](auto& poolSize) {
            return poolSize.type == binding.descriptorType;
        });

        if (it == poolSizes.end()) {
            poolSizes.push_back({
                .type            = binding.descriptorType,
                .descriptorCount = binding.descriptorCount * spec.descriptorSetCount,
            });
        } else {
            it->descriptorCount += binding.descriptorCount * spec.descriptorSetCount;
        }
    }

    const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = {},
        .maxSets       = spec.descriptorSetCount,
        .poolSizeCount = static_cast<uint32>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data(),
    };

    VkDescriptorPool descriptorPool;
    result = vkCreateDescriptorPool(m_device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
    ENSURE(result == VK_SUCCESS);
    VulkanObject<VkDescriptorPool>::m_handle = descriptorPool;

    // DescriptorSets
    const std::vector descriptorSetLayouts(spec.descriptorSetCount, VulkanObject<VkDescriptorSetLayout>::m_handle);

    const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = VulkanObject<VkDescriptorPool>::m_handle,
        .descriptorSetCount = spec.descriptorSetCount,
        .pSetLayouts        = descriptorSetLayouts.data(),
    };

    m_descriptorSets.resize(spec.descriptorSetCount);
    result = vkAllocateDescriptorSets(m_device, nullptr, m_descriptorSets.data());
    ENSURE(result == VK_SUCCESS);
}

DescriptorPool::~DescriptorPool() {
    // vkFreeDescriptorSets(m_device, vk(), static_cast<uint32>(m_descriptorSets.size()), m_descriptorSets.data());
    vkDestroyDescriptorSetLayout(m_device, VulkanObject<VkDescriptorSetLayout>::m_handle, nullptr);
    vkDestroyDescriptorPool(m_device, vk(), nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
