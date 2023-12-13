#if R3_VULKAN

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/DescriptorPool.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

DescriptorPool::DescriptorPool(const DescriptorPoolSpecification& spec)
    : m_spec(spec) {
    const VkDescriptorPoolSize uboDescriptorPoolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = m_spec.descriptorSetCount,
    };

    const VkDescriptorPoolSize samplerDescriptorPoolSize = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = m_spec.descriptorSetCount,
    };

    const VkDescriptorPoolSize poolSizes[2]{
        uboDescriptorPoolSize,
        samplerDescriptorPoolSize,
    };
    
    const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = m_spec.descriptorSetCount,
        .poolSizeCount = 2,
        .pPoolSizes = poolSizes,
    };

    VkDescriptorPool tmp;
    VkResult result =
        vkCreateDescriptorPool(m_spec.logicalDevice->as<VkDevice>(), &descriptorPoolCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);

    m_descriptorSets = DescriptorSet::allocate({
        .logicalDevice = m_spec.logicalDevice,
        .descriptorPool = this,
        .descriptorSetLayout = m_spec.descriptorSetLayout,
        .descriptorSetCount = m_spec.descriptorSetCount,
    });
}

DescriptorPool::~DescriptorPool() {
    if (validHandle()) {
        vkDestroyDescriptorPool(m_spec.logicalDevice->as<VkDevice>(), as<VkDescriptorPool>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN